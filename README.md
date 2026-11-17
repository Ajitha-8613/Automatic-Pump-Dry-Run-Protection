
# Automatic Pump Dry-Run Protection System

An Arduino-based automatic water pump protection system that detects insufficient water flow and automatically switches OFF the pump to prevent dry-running damage.

## 📌 Project Overview

The Automatic Pump Dry-Run Protection System is designed to protect a water pump from operating without sufficient water supply.

It uses a ZJ-S201 water flow sensor to continuously monitor the flow of water through the pump. When the detected flow remains below a predefined threshold for a specified duration, the system identifies a possible dry-run condition and automatically turns OFF the pump.

The system also includes a push button for starting and resetting the pump, along with LED indicators to display its operating status.

## 🎯 Objectives

- Detect insufficient water flow during pump operation.
- Prevent pump damage caused by dry running.
- Automatically switch OFF the pump when a dry-run condition is detected.
- Provide simple start and reset control using a single push button.
- Display pump status using LED indicators.

## 🛠️ Components Used

- Arduino board (Uno or compatible)
- ZJ-S201 Water Flow Sensor
- Single-channel relay module or suitable MOSFET driver
- Push Button
- Green LED (Running Indicator)
- Yellow LED (Fault Indicator)
- Resistors
- Connecting Wires
- Power Supply
- Water Pump

## 🔌 Pin Connections

| Component | Arduino Pin |
|---|---|
| Flow Sensor | D2 |
| Push Button | D3 |
| Pump Relay/MOSFET | D9 |
| Green LED | D5 |
| Yellow LED | D6 |

## ⚙️ Working Principle

1. When the system is powered ON, it enters IDLE mode.
2. Pressing the push button starts the water pump.
3. The system allows an initial 6-second startup period before monitoring begins.
4. The ZJ-S201 flow sensor generates pulses according to the water flow.
5. Arduino calculates the water flow rate in litres per minute (L/min).
6. If the flow rate remains below the predefined threshold for 2 seconds, a dry-run condition is detected.
7. The pump is automatically switched OFF.
8. The yellow LED indicates the fault condition.
9. Pressing the push button again resets the fault and returns the system to IDLE mode.

The flow is monitored continuously while the pump is running.

## ✨ Features

- Automatic dry-run detection
- Continuous water flow monitoring
- Automatic pump shutdown during low-flow conditions
- Single-button START and RESET operation
- Manual pump stopping
- LED-based status indication
- Serial Monitor output for flow rate monitoring

## 📊 System Parameters

| Parameter | Value |
|---|---|
| Flow Sensor | ZJ-S201 |
| Approx. Pulses per Litre | 450 |
| Flow Calculation Interval | 1 second |
| Dry-Run Threshold | 1.0 L/min |
| Low-Flow Confirmation Time | 2 seconds |
| Startup Delay | 6 seconds |

Note: Sensor calibration and flow thresholds may need adjustment depending on the actual sensor and pump.

## 💻 Software Requirements

- Arduino IDE
- Arduino-compatible board
- USB cable for programming

## 🚀 How to Run

1. Install the Arduino IDE.
2. Connect the Arduino board to your computer.
3. Open `pump_protection.ino`.
4. Select the correct Arduino board and COM port.
5. Upload the program.
6. Connect the components according to the pin connection table.
7. Open the Serial Monitor at 9600 baud to observe the flow rate and system status.

## 🔮 Future Improvements

- Add an LCD display to show real-time flow rate.
- Introduce IoT-based remote monitoring.
- Send mobile notifications during fault conditions.
- Store pump operating data for performance analysis.
- Add automatic fault alerts and reporting.