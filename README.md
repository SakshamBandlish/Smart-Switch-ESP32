# ESP32 Native Alexa Smart Switch

A clean, production-ready Internet of Things (IoT) firmware layout that turns an ESP32 microcontroller and a 2-channel relay module into a native smart home appliance. 

This project operates entirely over Wi-Fi via the **Sinric Pro** cloud platform. It integrates directly into the official Amazon Alexa application as an authorized smart device—**eliminating the need for custom mobile terminal apps, background macros, or local notification sniffing tools.**

---

## 🚀 Key Features

* **Zero-App Background Overheads:** Control hardware natively using standard Alexa voice commands or the official Alexa dashboard layout.
* **Boot-Safe Pin Routing:** Avoids hardware strapping pin conflicts during code uploads.
* **Persistent WebSockets:** Employs low-overhead cloud keep-alive packets for near-instant execution latency.
* **Active-LOW Architecture:** Pre-configured to align perfectly with standard multi-channel optical isolation relays.

---

## 📊 Circuit Pin Map

The system cleanly bifurcates the low-voltage DC logic control layer from high-voltage household electrical lines. 

### Low-Voltage Control Matrix (ESP32 ➔ Relay Board)
Wire the control inputs of your 2-channel relay module to the ESP32 development board as follows:

| Relay Module Pin | ESP32 Target Pin | Wiring Description |
| :--- | :--- | :--- |
| **VCC** | **VIN** (or **5V**) | Routes 5V USB bus power directly to the relay activation coils |
| **GND** | **GND** | Establishes common system logic ground reference |
| **IN1** | **GPIO 4** (Labeled **D4** or **G4**) | Independent logic trigger line for Relay Channel 1 |
| **IN2** | *Leave Disconnected* | Unused secondary relay logic channel |

> ⚠️ **STRAPPING PIN CAUTION:** Do **NOT** connect the relay control line to GPIO 2. GPIO 2 acts as an internal hardware boot strap. If the relay's internal pull-up resistor or optocoupler circuit pulls this pin high during power-up, the ESP32 will fail to enter flash mode, resulting in serial upload timeouts (`Wrong boot mode detected (0xb)`). Always run control signals on an independent pin like **GPIO 4**.

---

## 🔍 Troubleshooting & Hard Lessons Learned

During development, this project encountered real-world hardware and safety challenges. Below are the specific edge-cases diagnosed and resolved:

### 1. ESP32 Boot Mode Failures (`Wrong boot mode detected (0xb)`)
* **The Problem:** When uploading firmware via the Arduino IDE, the console repeatedly threw a fatal error claiming the chip was in the wrong boot mode, even when manually holding the physical `BOOT` button down.
* **The Cause:** The relay module's `IN1` signal wire was initially plugged into **GPIO 2**. On the ESP32 architecture, GPIO 2 is a *strapping pin* read by the chip during its exact boot sequence. The internal circuitry of the multi-channel relay pulled this pin voltage up, blocking the ESP32 from entering download mode.
* **The Resolution:** Swapped the control signal entirely from GPIO 2 over to **GPIO 4**, an independent, non-strapping pin. Code flashes instantly without interfering with the hardware boot state.

### 2. Missing USB-to-UART Bridge Drivers
* **The Problem:** The ESP32 dev board was entirely missing from the Arduino IDE `Tools ➔ Port` menu upon first connection, despite using a functioning data cable.
* **The Cause:** Modern computers do not always carry native virtual COM port mappings for low-cost Chinese USB-serial chips commonly found on microcontrollers. 
* **The Resolution:** Identifed the controller footprint chip on the board board (Silicon Labs `CP2102`) and installed the dedicated **CP210x Universal Windows Driver (`silabser.inf`)** to allow proper OS COM communication.

### 3. Active-LOW Logic Inversion
* **The Problem:** Triggering Alexa to turn the smart switch "ON" physically turned the attached appliance off, and vice versa.
* **The Cause:** Standard multi-channel relay modules operate on *Active-LOW* logic principles, meaning grounding the pin (`LOW`) fires the internal mechanical coil, whereas driving it to `HIGH` opens the circuit.
* **The Resolution:** Inverted the firmware execution parameters inside the `onPowerState` cloud callback loop, ensuring a `true` state pushes a `LOW` write, matching human operational expectations.

### 4. Critical High-Voltage Mechanical Failure & Burn Incident
* **The Danger:** During live desk testing, a loose AC wire carrying **230V mains current** slipped completely free from the relay module's screw terminal block and brushed against the exposed traces underneath the PCB. This immediately energized the entire development platform, resulting in a **severe electrical shock and skin entry burns** upon handling.
* **The Core Architectural Fixes:**
  * **Strict Mechanical Strain Relief:** Frayed, bare stranded copper wiring must never be placed raw inside high-voltage screw terminal blocks. All high-voltage AC connections must be tightly twisted, properly insulated, or crimped into dedicated wire ferrules to prevent rogue copper strands from slipping free.
  * **Mandatory Enclosure Isolation:** A high-voltage relay module must *never* be tested or run open on a workspace desk. The entire component framework must be permanently isolated within a non-conductive plastic box or a 3D-printed project enclosure to prevent any possibility of accidental physical contact with live elements.
 
  * ---

## 🛠️ Software Stack Setup

### 1. Cloud Infrastructure Integration
1. Register a free account at **[sinric.pro](https://sinric.pro)**.
2. From the main dashboard panel, go to **Devices** ➔ **Add Device**.
3. Define the device name as `Smart Switch` (this acts as your default Alexa voice tag).
4. Set the device type to **Switch** and click **Save**.
5. Copy your three tracking parameters: **App Key**, **App Secret**, and **Device ID**.

### 2. Environment Configuration (Arduino IDE)
1. Launch the **Arduino IDE**, navigate to **File** ➔ **Preferences**.
2. Paste the following URL into the **Additional Boards Manager URLs** input box:
   ```text
   [https://espressif.github.io/arduino-esp32/package_esp32_index.json](https://espressif.github.io/arduino-esp32/package_esp32_index.json)
