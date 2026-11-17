/*
  Automatic Pump Dry-Run Protection System (single pump, one button)
  ---------------------------------------------------------------------
  Flow sensor: ZJ-S201 (1-30 L/min rated, ~450 pulses/litre,
  frequency Hz approx 7.5 x flow rate in L/min)

  One push button acts as both START and RESET:
    - Press while IDLE  -> starts the pump and begins monitoring.
    - Press while FAULT -> clears the fault, returns to IDLE.
    - Press while RUNNING -> stops the pump manually (optional; see note).

  Flow is checked continuously the whole time the pump runs, so a
  dry-run that develops mid-operation is caught, not just one that
  exists at the moment the button is pressed.

  Pin plan:
    D2 - Flow sensor pulse output (interrupt)
    D3 - Push button (INPUT_PULLUP, other leg to GND)
    D9 - Pump relay/MOSFET
    D5 - Green LED (running)   [optional, remove if unused]
    D6 - Yellow LED (fault)    [optional, remove if unused]
*/

const int FLOW_SENSOR_PIN = 2;
const int BUTTON_PIN      = 3;
const int PUMP_PIN        = 9;
const int GREEN_LED_PIN   = 5;
const int YELLOW_LED_PIN  = 6;

volatile unsigned long pulseCount = 0;

// ---- Calibration ----
const float PULSES_PER_LITRE = 450.0;   // ZJ-S201 approx figure; confirm with your unit

// ---- Thresholds (tune to your pump's real flow) ----
const float DRY_RUN_THRESHOLD_LPM = 1.0;
const unsigned long FLOW_CALC_INTERVAL_MS = 1000;
const unsigned long DRY_RUN_CONFIRM_MS    = 2000;  // low flow must persist this long to trip

const unsigned long DEBOUNCE_MS = 250;

enum State { IDLE, RUNNING, FAULT };
State state = IDLE;

unsigned long stateEnteredAt = 0;
unsigned long lastFlowCalcAt = 0;
unsigned long lowFlowSince = 0;
bool lowFlowActive = false;
float currentFlowLpm = 0.0;

unsigned long lastButtonPress = 0;
unsigned long lastBlink = 0;
bool yellowState = false;

void flowPulse() {
  pulseCount++;
}

void setup() {
  Serial.begin(9600);

  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);

  digitalWrite(PUMP_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(YELLOW_LED_PIN, LOW);

  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), flowPulse, FALLING);

  Serial.println(F("Pump Protection System ready. Press button to start."));
}

void loop() {
  handleButton();
  updateFlowRate();
  runStateMachine();
}

void handleButton() {
  if (digitalRead(BUTTON_PIN) == LOW && millis() - lastButtonPress > DEBOUNCE_MS) {
    lastButtonPress = millis();

    if (state == IDLE) {
      digitalWrite(PUMP_PIN, HIGH);
      Serial.println(F("Pump ON. Waiting 6 seconds before monitoring..."));
      delay(6000);   // pump runs for 6 s before flow is checked
      enterState(RUNNING);
    } else if (state == FAULT) {
      enterState(IDLE);
    } else if (state == RUNNING) {
      enterState(IDLE);   // manual stop; remove this branch if you don't want it
    }
  }
}

void updateFlowRate() {
  if (millis() - lastFlowCalcAt >= FLOW_CALC_INTERVAL_MS) {
    noInterrupts();
    unsigned long pulses = pulseCount;
    pulseCount = 0;
    interrupts();

    float intervalMin = (millis() - lastFlowCalcAt) / 60000.0;
    currentFlowLpm = (pulses / PULSES_PER_LITRE) / intervalMin;
    lastFlowCalcAt = millis();

    Serial.print(F("Flow: "));
    Serial.print(currentFlowLpm);
    Serial.println(F(" L/min"));
  }
}

void runStateMachine() {
  switch (state) {

    case IDLE:
      digitalWrite(PUMP_PIN, LOW);
      digitalWrite(GREEN_LED_PIN, LOW);
      digitalWrite(YELLOW_LED_PIN, LOW);
      break;

    case RUNNING:
      digitalWrite(GREEN_LED_PIN, HIGH);
      digitalWrite(YELLOW_LED_PIN, LOW);

      if (currentFlowLpm < DRY_RUN_THRESHOLD_LPM) {
        if (!lowFlowActive) {
          lowFlowActive = true;
          lowFlowSince = millis();
        } else if (millis() - lowFlowSince >= DRY_RUN_CONFIRM_MS) {
          enterState(FAULT);
        }
      } else {
        lowFlowActive = false;
      }
      break;

    case FAULT:
      digitalWrite(PUMP_PIN, LOW);
      digitalWrite(GREEN_LED_PIN, LOW);
      blinkYellow();
      break;
  }
}

void enterState(State newState) {
  state = newState;
  stateEnteredAt = millis();
  lowFlowActive = false;

  switch (newState) {
    case IDLE:     Serial.println(F("Idle. Press button to start.")); break;
    case RUNNING:  Serial.println(F("Running normally.")); break;
    case FAULT:    Serial.println(F("* DRY-RUN DETECTED: pump OFF *")); break;
  }
}

void blinkYellow() {
  const unsigned long blinkMs = 300;
  if (millis() - lastBlink >= blinkMs) {
    lastBlink = millis();
    yellowState = !yellowState;
    digitalWrite(YELLOW_LED_PIN, yellowState ? HIGH : LOW);
  }
}