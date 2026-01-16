# Flight Telemetry System (FTS) - CubeSat Simulation

A deterministic, multithreaded flight software prototype designed for a simulated ARM Cortex-M3 environment. This system models the core telemetry and fault-tolerance logic required for autonomous satellite operations.

## 🚀 Overview
This project implements a real-time "Satellite-on-a-Chip" architecture using **FreeRTOS**. It simulates orbital physics (thermal cycles and power consumption) and manages data downlink via a prioritized task system. 

### Key Features
* **Multithreaded Architecture:** 5 concurrent tasks managed by the FreeRTOS preemptive scheduler.
* **Fault Tolerance:** Implements a **Software Watchdog Timer** and autonomous **Safe Mode** recovery logic.
* **Data Integrity:** Integrates **CRC-16-CCITT Checksums** to detect bit-flips in telemetry packets.
* **Inter-Task Communication:** Uses thread-safe **Queues** for sensor telemetry and **Mutexes** for UART resource protection.

## 🏗️ System Architecture

### Task Breakdown
| Task | Priority | Responsibility |
| :--- | :--- | :--- |
| **Watchdog** | High | Monitors task check-ins; triggers system reset on hung tasks. |
| **Command** | Mid | Interrupt-driven UART handler for manual ground commands (Heater, Chaos, Reset). |
| **Telemetry** | Mid | Validates packet integrity (CRC) and formats data for downlink. |
| **Sensor** | Mid | Samples simulated hardware and pushes packets to the queue. |
| **Environment**| Low | Models orbital physics (Sun/Eclipse cycles). |

## 🛡️ Reliability & Data Integrity
The system is designed with a "Safety-First" mindset:
1. **Watchdog Timer:** If a task enters an infinite loop (e.g., via the `k` chaos command), the Watchdog initiates a fail-safe reboot within 3000ms.
2. **CRC-16 Validation:** Before downlinking, the Telemetry task calculates a checksum of the struct. This ensures that only uncorrupted data is processed—a requirement for noisy space-to-ground links.
3. **Resource Guarding:** All shared hardware (UART) is protected by Mutexes to prevent race conditions.

## 🛠️ Technical Stack
* **Language:** Embedded C
* **Operating System:** FreeRTOS
* **Target Hardware (Emulated):** ARM Cortex-M3
* **Toolchain:** GCC ARM Embedded Toolchain (`arm-none-eabi-gcc`)
* **Simulation/Debug:** QEMU, GDB

## 🔧 Usage
1. **Build:** `make`
2. **Run:** `make run`
3. **Commands:**
    * `h`: Toggle heater.
    * `k`: Inject sensor fault (Chaos Mode).
    * `z`: Manual system recovery.
