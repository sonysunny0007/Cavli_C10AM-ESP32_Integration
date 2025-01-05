# ESP32 + Cavli C10QM Integration for Cellular Connectivity

This project demonstrates how to integrate the ESP32 with the Cavli C10QM cellular modem to enable cellular connectivity for various applications. It includes code for modem initialization, network attachment, and packet data communication.

## Features

- **Modem Initialization**: Reset, wake up, and initialize the Cavli C10QM modem.
- **Network Attachment**: Automatic or manual network registration.
- **Packet Data Connection**: Enable and monitor GPRS/packet data connection.
- **MQTT Connectivity**: Connect to an MQTT broker and send messages.
- **SMS (Optional)**: Read and send SMS using the cellular modem (commented out in the code).
- **Networking Information**: Retrieve and display modem information, signal quality, and IP address.

## Hardware Setup

### Connections

- **ESP32 TX (IO26)** → **Cavli C10QM RX**
- **ESP32 RX (IO27)** → **Cavli C10QM TX**
- **ESP32 IO13** → **Cavli C10QM RESET**
- **ESP32 IO12** → **Cavli C10QM PWRKEY**
- **ESP32 GND** → **Cavli C10QM GND**

Ensure that the **SIM card** is properly inserted into the Cavli C10QM module.

## Software Requirements

- **Arduino IDE** with the ESP32 board package installed
- **Cavli C10QM Firmware** and necessary libraries (like `C10AMCore.h`)

## Setup Instructions

1. **Hardware Setup**
   - Connect the **ESP32** and **Cavli C10QM** following the wiring diagram above.
   - Ensure that the **SIM card** is properly inserted into the Cavli C10QM module.

2. **Configure Arduino IDE**
   - Install the **ESP32 board package** in the Arduino IDE.
   - Select the **ESP32 board** and set the following configurations:
     - **Upload Speed**: 115200
     - **Flash Size**: 4MB (or your specific size)

3. **Upload the Firmware**
   - Press and hold the **BOOT button** on the ESP32.
   - While holding the **BOOT button**, press and release the **RESET button** to enter **BOOT mode**.
   - Upload the firmware to the ESP32 using the Arduino IDE.

4. **Monitor the Output**
   - Open the **Serial Monitor** in the Arduino IDE to observe the output.
   - It will show the modem's initialization, network attachment, and the packet data status.

## Code Overview

The code performs the following steps:

1. **Modem Initialization**: Resets and wakes up the modem, then waits for it to become available.
2. **Network Attachment**: Waits for the modem to attach to the network. If not automatically attached, it can attempt to manually set the network.
3. **Packet Data Connection**: Enables packet data and checks the connection status.
4. **MQTT Connectivity**: Creates an MQTT connection to the broker and attempts to publish a message (additional code for publishing larger messages is commented out).
5. **Signal Quality**: Displays RF signal strength, RSSI, and BER values.

## Troubleshooting

- If the modem is not found, check the wiring and ensure the **Cavli C10QM** is powered correctly.
- If the network connection fails, ensure that the **SIM card** is active and supports cellular data.
- Monitor the **Serial Monitor** for any error messages and debug accordingly.

## License

This project is licensed under the terms of the **MIT License**.
