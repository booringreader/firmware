# Secure Bare--Metal Bootloader & Firmware Update System for STM32

![Language](https://img.shields.io/badge/language-C-blue.svg)
![Platform](https://img.shields.io/badge/platform-Bare--Metal-lightgrey.svg)
![Microcontroller](https://img.shields.io/badge/MCU-STM32%20Cortex--M4-orange.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)

## Overview

This project is a complete, bare-metal firmware system for an STM32 Cortex-M4 microcontroller, developed from the ground up without reliance on vendor HALs or an operating system. The core of this project is a secure bootloader that enables robust and reliable in-field firmware updates over a UART interface.

The system is architected to be resilient against common failure modes like update interruptions and to be secure against the execution of unauthorized or malicious code. It demonstrates advanced embedded systems concepts including memory management, custom driver development, communication protocols, and applied cryptography.

## Key Features

-   **Secure Bootloader:** A small, reliable bootloader that manages the application lifecycle.
-   **In-Field Firmware Updates:** Update the main application firmware via UART without needing a dedicated programmer.
-   **Multi-Layered Security:**
    -   **Integrity Check (CRC32):** Ensures the firmware image is not corrupted or incomplete.
    -   **Authenticity Check (AES CBC-MAC):** Cryptographically verifies that the firmware is from a trusted source.
-   **Resilient Communication Protocol:** A custom packet-based protocol with CRC8 checksums, acknowledgments, and retransmissions for reliable data transfer.
-   **Bare-Metal Development:** All drivers (GPIO, UART, Flash) and system logic are written from scratch to directly interface with the hardware.
-   **Precise Memory Management:** Utilizes custom linker scripts to partition flash memory and manage the memory layout for the bootloader and application.

---

## Firmware Architecture

The firmware is divided into two distinct components that reside in separate regions of the microcontroller's flash memory. This separation is key to the system's robustness.

```

\+--------------------------------+  \<-- Flash Start (0x08000000)
|                                |
|      Bootloader (32 KB)        |
|  (Initializes, Validates,      |
|   Handles Updates)             |
|                                |
\+--------------------------------+  \<-- App Start (0x08008000)
|                                |
|      Main Application          |
|  (Device Core Logic,           |
|   Can be Updated)              |
|                                |
|                                |
|                                |
\+--------------------------------+  \<-- Flash End

```

### 1. The Bootloader

The bootloader is the first code to execute on power-up. Its responsibilities are:
-   Initialize essential hardware (clocks, GPIO, UART).
-   Perform a rigorous security check on the main application stored in flash.
-   If the application is valid and authentic, it jumps to the application's entry point.
-   If the application is invalid or a firmware update is triggered, it enters update mode, communicating with a host PC to receive a new firmware image.

### 2. The Main Application

This component contains the primary logic of the device.
-   It is completely independent of the bootloader.
-   It has its own interrupt vector table. A critical step in the bootloader-to-application transition is **Vector Table Relocation**, where the `VTOR` (Vector Table Offset Register) is updated to point to the application's vector table. This ensures interrupts are handled by the application, not the bootloader.

---

## Project Directory Structure

```

.
├── app/                  \# Main Application
│   ├── src/
│   ├── inc/
│   └── stm32f4.ld        \# Application Linker Script
├── bootloader/           \# Secure Bootloader
│   ├── src/
│   ├── inc/
│   └── stm32f4.ld        \# Bootloader Linker Script
├── shared/               \# Code shared between bootloader and app
│   ├── src/
│   └── inc/
├── tools/                \# Host-side helper scripts
│   ├── pad\_bootloader.py
│   └── firmware\_signer.py
├── libopencm3/           \# Git submodule for the low-level library
└── Makefile              \# Top-level makefile for building the project

````

---

## Security Deep Dive

The security of the system is built on two layers, each protecting against a different threat.

### Layer 1: Integrity Check with CRC32

-   **Threat:** An incomplete or corrupted firmware image caused by a power failure or communication error during an update.
-   **Solution:** Before transmission, the host PC calculates a **CRC32 checksum** of the firmware binary. This checksum is embedded within a metadata struct (`firmware_info`) inside the binary itself.
-   **Validation:** Upon boot, the bootloader performs the same CRC32 calculation on the stored application firmware. It fails the boot process if its calculated checksum does not match the checksum stored in the metadata. This prevents the execution of a damaged firmware image.

### Layer 2: Authenticity Check with AES CBC-MAC

-   **Threat:** A malicious actor creating their own firmware and trying to load it onto the device. A simple CRC check is not sufficient as an attacker can easily calculate a valid CRC for their malicious code.
-   **Solution:** A **Message Authentication Code (MAC)** is generated using the **AES-128 CBC-MAC** algorithm.
-   **Process:**
    1.  A **secret key** is compiled into the bootloader and is also known by the developer's signing tool. This key **never** leaves the device or the secure development environment.
    2.  The `firmware_signer.py` script uses this key to generate a 16-byte cryptographic signature (the MAC) of the firmware.
    3.  This signature is embedded into the firmware's metadata struct.
    4.  Upon boot, the bootloader uses its stored secret key to calculate the MAC of the application firmware.
    5.  It compares the calculated MAC with the one stored in the metadata. If they do not match, the bootloader knows the firmware is either not from the trusted source or has been tampered with, and it will refuse to execute it.

---

## Build and Deployment Workflow

The end-to-end process of building and deploying a secure firmware update is as follows:

1.  **Compile:** Use `make` to compile both the bootloader and the main application. The makefile is configured to produce separate `.bin` files for each.
2.  **Pad Bootloader:** The bootloader binary is padded to its exact reserved size (e.g., 32KB) using `pad_bootloader.py`.
3.  **Embed Bootloader:** The padded bootloader binary is embedded into the main application's `.elf` file in its own linker section. This creates a single, combined binary that can be flashed initially with a programmer.
4.  **Sign Application:** For a firmware update, the application's `.bin` file is processed by the `firmware_signer.py` script. This script:
    -   Calculates the CRC32 and embeds it.
    -   Calculates the AES CBC-MAC signature and embeds it.
    -   Produces a final, signed `.bin` file ready for deployment.
5.  **Deploy Update:** A host-side updater application (e.g., a Python or TypeScript script) sends the signed `.bin` file to the device's bootloader over UART, following the custom communication protocol. The bootloader receives, validates, and writes the new firmware to flash.

---

## How to Build

### Prerequisites

-   ARM GCC Toolchain (`arm-none-eabi-gcc`)
-   `make`
-   Python 3

### Build Steps

```bash
# Clone the repository
git clone <your-repo-url>
cd <your-repo-name>

# Initialize and fetch the libopencm3 submodule
git submodule init
git submodule update

# Build the entire project (bootloader and application)
make
````