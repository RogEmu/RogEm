# RogEm - Technical Documentation: Technology, Architecture and Implementation Choices

## Table of Contents

1. [Technology Choices](#1-technology-choices)
   - 1.1 [Language and Standard](#11-language-and-standard)
   - 1.2 [C++ Libraries](#12-c-libraries)
   - 1.3 [Build Tools](#13-build-tools)
   - 1.4 [Quality and Documentation Tools](#14-quality-and-documentation-tools)
   - 1.5 [CI/CD and Infrastructure](#15-cicd-and-infrastructure)
2. [Architecture Choices](#2-architecture-choices)
   - 2.1 [Project Organization](#21-project-organization)
   - 2.2 [Emulation Core Architecture](#22-emulation-core-architecture)
   - 2.3 [The Bus as Central Mediator](#23-the-bus-as-central-mediator)
   - 2.4 [Memory Mapping](#24-memory-mapping)
   - 2.5 [CPU Architecture](#25-cpu-architecture)
   - 2.6 [GPU Architecture](#26-gpu-architecture)
   - 2.7 [DMA System](#27-dma-system)
   - 2.8 [Interrupt Controller](#28-interrupt-controller)
   - 2.9 [GUI and Debugger Architecture](#29-gui-and-debugger-architecture)
   - 2.10 [Save State System](#210-save-state-system)
3. [Implementation Choices](#3-implementation-choices)
   - 3.1 [Design Patterns Used](#31-design-patterns-used)
   - 3.2 [MIPS Instruction Pipeline](#32-mips-instruction-pipeline)
   - 3.3 [GTE (Geometry Transform Engine)](#33-gte-geometry-transform-engine)
   - 3.4 [GPU Rasterization](#34-gpu-rasterization)
   - 3.5 [Input Handling (Controller)](#35-input-handling-controller)
   - 3.6 [libcuebin: CD-ROM Library](#36-libcuebin-cd-rom-library)
   - 3.7 [Testing Strategy](#37-testing-strategy)

---

## 1. Technology Choices

### 1.1 Language and Standard

| Choice | Detail |
|--------|--------|
| **C++20** | Language standard |
| **GCC / G++** | Linux compiler |
| **MSVC (cl)** | Windows compiler |

**Rationale**: C++ is the reference language for game console emulation due to its near-hardware performance and fine-grained memory control. The C++20 standard brings modern features (concepts, extended constexpr, `std::span`, designated initializers) that improve code readability and safety without sacrificing performance. Dual GCC/MSVC support ensures cross-platform portability.

### 1.2 C++ Libraries

All external dependencies are managed through **vcpkg** and declared in `vcpkg.json`:

| Library | Role | Rationale |
|---------|------|-----------|
| **fmt** | String formatting | Faster than `std::format` (C++20), more expressive API, widely adopted in the C++ ecosystem |
| **spdlog** | Logging | Built on fmt, supports multiple sinks (console, file), performant thanks to asynchronous buffering, header-only option available |
| **glfw3** | Window + OpenGL context + input | Lightweight, cross-platform (Linux/Windows/macOS), de facto standard for OpenGL applications, no heavy dependency like Qt or SDL |
| **glad** (feature: `gl-api-33`) | OpenGL 3.3 function loading | Loader generator targeting a precise OpenGL version, ensures compatibility with target hardware (OpenGL 3.3 Core = broad hardware coverage) |
| **imgui** (features: `glfw-binding`, `opengl3-binding`, `docking-experimental`) | Graphical interface (debugger) | Immediate mode is ideal for development/debug tools that change every frame, docking for multi-panel windows, very lightweight and requires no heavy framework |
| **capstone** (feature: `mips`) | MIPS instruction disassembly | Reference library in the reverse engineering field, native MIPS R3000 instruction set support, reliable and well-maintained |
| **nlohmann-json** | JSON parsing/serialization | Intuitive API with natural C++ syntax, header-only, de facto standard for JSON in C++ |
| **argparse** | CLI argument parsing | Lightweight, modern API (C++17+), eliminates the need to manually parse `argv` |
| **gtest** (Google Test) | Unit testing framework | Most widely used C++ testing framework, excellent CMake support (`gtest_discover_tests`), expressive macros (`EXPECT_EQ`, `ASSERT_TRUE`), test fixtures |

**Custom library: libcuebin**

| Choice | Detail |
|--------|--------|
| **libcuebin** | In-house C++20 library for CUE file parsing and raw CD-ROM sector reading |

**Rationale**: Existing libraries for CUE/BIN parsing are either written in C with a low-level API or do not cover the specifics of the PS1 CD-ROM format (multiple sector modes, MSF addressing, pregap/postgap). Developing an in-house library provides full control over the format, a modern C++20 API with `Result<T>` error handling (no exceptions), and seamless integration with the project's architecture.

### 1.3 Build Tools

| Tool | Role | Rationale |
|------|------|-----------|
| **CMake** (>= 3.20) | Build system | De facto standard for cross-platform C++, native vcpkg support via `CMAKE_TOOLCHAIN_FILE`, generates for both Ninja and MSBuild |
| **vcpkg** | Dependency manager | Maintained by Microsoft, reproducible dependency resolution and builds, declarative `vcpkg.json` file that locks dependencies |
| **Ninja** (Linux) | Build generator | Faster than Make thanks to optimal parallelization and minimal parsing overhead |
| **MSBuild** (Windows) | Build generator | Native Visual Studio generator, seamless integration with MSVC |

**Overall rationale**: The CMake + vcpkg + Ninja/MSBuild combination is the most common and robust build toolchain in the modern C++ ecosystem. It allows any new contributor to clone the repo and build in 3 commands (`git submodule update --init`, `cmake -B build`, `cmake --build build`) without manually installing any dependency.

### 1.4 Quality and Documentation Tools

| Tool | Role | Rationale |
|------|------|-----------|
| **clang-format** | Automatic code formatting | Enforces a consistent code style across the entire project (C++20, 150 columns, custom style). Eliminates style debates in code reviews |
| **Doxygen** | Source code documentation generation | Standard for C++ documentation, generates navigable HTML from in-code comments |
| **gcov / gcovr** | Code coverage (optional, `ENABLE_COVERAGE=ON`) | Measures which lines of code are covered by unit tests |
| **Strict compiler flags** | `-Wall -Wextra -Wpedantic -Werror` (GCC), `/W4 /WX` (MSVC) | Treats all warnings as errors, forces resolution of every potential issue at compile time |

### 1.5 CI/CD and Infrastructure

| Tool | Role | Rationale |
|------|------|-----------|
| **GitHub Actions** | CI/CD | Free for public repos (2000 min/month), native GitHub integration, declarative YAML workflows |
| **GitHub Releases** + `softprops/action-gh-release` | Binary distribution | Worldwide CDN, automatic release creation on tag push, auto-generated release notes |
| **`pixta-dev/repository-mirroring-action`** | Mirror to EPITECH repo | Automatic sync on push to `main`, no manual intervention required |
| **`actions/cache`** | vcpkg caching | Speeds up CI builds by caching vcpkg binaries, key based on `vcpkg.json` hash |

**Branching strategy**:
- `features`: development branches
- `dev`: integration branch, protected by CI (build + tests on every PR)
- `main`: release branch, triggers the EPITECH mirror and releases on tag push

**Versioning**: Semantic Versioning (`vMAJOR.MINOR.PATCH`), history from v0.1.0 to v0.8.0.

---

## 2. Architecture Choices

### 2.1 Project Organization

```
rogem/
├── src/                      # Application source code
│   ├── main.cpp              # Entry point
│   ├── Application.cpp/hpp   # Main loop, GLFW/ImGui management
│   ├── Core/                 # rgmcore static library (hardware emulation)
│   │   ├── CPU.cpp/hpp       # MIPS R3000A processor
│   │   ├── GPU.cpp/hpp       # Graphics processor
│   │   ├── GTE.cpp/hpp       # Geometry Transform Engine (COP2)
│   │   ├── Bus.cpp/hpp       # Central bus, communication mediator
│   │   ├── DMA.cpp/hpp       # Direct Memory Access
│   │   ├── System.cpp/hpp    # Emulation orchestrator
│   │   ├── Memory.cpp/hpp    # Abstract memory class
│   │   ├── BIOS.cpp/hpp      # BIOS ROM (512 KB, read-only)
│   │   ├── RAM.cpp/hpp       # Main RAM (2 MB)
│   │   ├── ScratchPad.cpp/hpp # Fast scratchpad memory (1 KB)
│   │   ├── MemoryMap.cpp/hpp # MIPS address segment mapping
│   │   ├── SPU.cpp/hpp       # Sound Processing Unit (stub)
│   │   ├── Timers.cpp/hpp    # 3 hardware timers
│   │   ├── InterruptController.cpp/hpp
│   │   └── ...
│   ├── Debugger/             # Interactive debugger
│   │   ├── Debugger.cpp/hpp
│   │   └── Disassembler.cpp/hpp
│   └── GUI/                  # ImGui windows
│       ├── AssemblyWindow.cpp/hpp
│       ├── RegisterWindow.cpp/hpp
│       ├── MemoryWindow.cpp/hpp
│       ├── BreakpointWindow.cpp/hpp
│       ├── LogWindow.cpp/hpp
│       └── ...
├── tests/                    # Google Test unit tests
├── lib/libcuebin/            # CUE/BIN library (submodule)
├── vcpkg/                    # Dependency manager (submodule)
└── docs/                     # Internal documentation
```

**Rationale**: The project is split into two distinct CMake targets:
- **`rgmcore`** (static library): contains all emulation logic with no graphical dependency. Can be tested independently.
- **`rogem`** (executable): links `rgmcore` with graphical libraries (GLFW, ImGui, OpenGL) for the user interface.

This separation allows testing the emulation core without launching a graphical interface, and potentially reusing `rgmcore` in a different frontend (headless, SDL, etc.).

### 2.2 Emulation Core Architecture

The core architecture mirrors the actual PS1 hardware topology:

```
                    ┌─────────────────────────────────────────────────────┐
                    │                     System                          │
                    │  (orchestrator: cycle loop, save states)            │
                    └────────────┬────────────────────────┬───────────────┘
                                 │                        │
                           ┌─────▼─────┐          ┌──────▼──────┐
                           │    CPU     │          │     GTE     │
                           │ MIPS R3000 │◄────────►│    (COP2)   │
                           │  + COP0    │          │  Geometry   │
                           └─────┬──────┘          └─────────────┘
                                 │
                           ┌─────▼─────┐
                           │    Bus     │
                           │ (mediator) │
                           └─────┬──────┘
              ┌──────┬──────┬────┴────┬──────┬──────┬────────────┐
              ▼      ▼      ▼         ▼      ▼      ▼            ▼
           ┌─────┐┌─────┐┌────┐  ┌──────┐┌─────┐┌──────┐┌────────────┐
           │BIOS ││ RAM ││SPad│  │ GPU  ││ DMA ││ SPU  ││  Timers    │
           │512KB││ 2MB ││1KB │  │VRAM  ││7 ch.││(stub)││ 3 channels │
           └─────┘└─────┘└────┘  └──────┘└─────┘└──────┘└────────────┘
                                                    │
                                          ┌─────────▼─────────┐
                                          │InterruptController│
                                          │   IRQ dispatch     │
                                          └───────────────────┘
```

**Rationale**: Modeling the software architecture as a mirror of the actual hardware offers several advantages:
- **Comprehension**: any developer familiar with the PS1 recognizes the same topology
- **Isolation**: each component is self-contained and independently testable
- **Extensibility**: adding a new peripheral (CDROM, Memory Card) consists of creating a new class inheriting from `PsxDevice` and registering it on the Bus

### 2.3 The Bus as Central Mediator

The Bus is the central point for all inter-component communication. All peripherals are registered in an `std::unordered_map<std::type_index, std::unique_ptr<PsxDevice>>`:

```cpp
// Register a peripheral
template<typename T>
void Bus::addDevice(std::unique_ptr<T> device);

// Retrieve by type
template<typename T>
T *Bus::getDevice();
```

**Memory access flow**:
1. The CPU calls `m_bus->loadWord(address)` or `m_bus->storeWord(address, value)`
2. The Bus converts the virtual address to a physical address via `MemoryMap::mapAddress()`
3. The Bus iterates its peripherals and calls `device->isAddressed(physicalAddr)`
4. The target peripheral receives the `read32()`/`write32()` call with the local offset

**Rationale for the type-indexed registry**:
- O(1) access via `type_index` hash for direct calls (`getDevice<GPU>()`)
- Only one instance per peripheral type (matches PS1 hardware reality)
- No arbitrary numeric IDs; the C++ type system guarantees consistency

### 2.4 Memory Mapping

The MIPS R3000 divides the 32-bit address space into segments:

| Segment | Address Range | Property |
|---------|---------------|----------|
| **KUSEG** | `0x00000000 - 0x7FFFFFFF` | User space, direct mapping |
| **KSEG0** | `0x80000000 - 0x9FFFFFFF` | Kernel, cache enabled, masks the top 3 bits |
| **KSEG1** | `0xA0000000 - 0xBFFFFFFF` | Kernel, no cache, masks the top 3 bits |
| **KSEG2** | `0xFFFE0000 - 0xFFFFFFFF` | Cache control registers |

The implementation uses a lookup table indexed by the top 3 bits to determine the region and compute the physical address. Each peripheral's memory ranges are declared as `constexpr`:

```cpp
constexpr MemRange BIOS_RANGE       = {0x1FC00000, 512 * 1024};
constexpr MemRange RAM_RANGE        = {0x00000000, 2048 * 1024};
constexpr MemRange SCRATCHPAD_RANGE = {0x1F800000, 1024};
constexpr MemRange GPU_RANGE        = {0x1F801810, 0x10};
constexpr MemRange DMA_RANGE        = {0x1F801080, 0x80};
```

**Rationale**: This compile-time approach with `constexpr` avoids any dynamic allocation and makes the mapping verifiable at compile time. The 3-bit lookup table is O(1) for segment resolution.

### 2.5 CPU Architecture

The MIPS R3000A CPU is implemented as a monolithic class with:
- 32 general-purpose registers (GPR) + PC, HI, LO
- A system coprocessor COP0 (`SystemControlCop`) for exceptions and privileges
- A GTE (COP2) for geometric operations

Instructions are decoded via a `union Instruction` that overlays the three MIPS formats:

```cpp
union Instruction {
    uint32_t raw;
    struct { uint32_t funct:6, shamt:5, rd:5, rt:5, rs:5, opcode:6; } r;   // R-type
    struct { uint32_t immediate:16, rt:5, rs:5, opcode:6; }           i;   // I-type
    struct { uint32_t address:26, opcode:6; }                          j;   // J-type
};
```

**Rationale**: The union allows decoding any instruction by reading a single 32-bit word, without manual bitmasks. The code for individual instructions (`addImmediate()`, `branchOnEqual()`, etc.) remains readable.

### 2.6 GPU Architecture

The GPU operates on a 1 MB VRAM (1024x512 pixels, ABGR1555 format) stored in `std::array<uint8_t, GPU_VRAM_1MB_SIZE>`.

It receives commands through two ports:
- **GP0** (`0x1F801810`): rendering commands (polygons, lines, rectangles, VRAM copies)
- **GP1** (`0x1F801814`): control commands (display mode, reset, DMA)

The GPU operates as a state machine:
```
WaitingForCommand -> ReceivingParameters -> ReceivingDataWords -> WaitingForCommand
```

Rendering is performed in software (CPU rasterization) and the VRAM is uploaded as an OpenGL texture each frame for display.

**Rationale**: Software rasterization is the standard choice for PS1 emulators as it faithfully reproduces the pixel-perfect behavior of the original hardware (no texture filtering, no antialiasing). The OpenGL upload is used solely for final display, not for computation.

### 2.7 DMA System

The DMA manages 7 channels corresponding to PS1 peripherals:

| Channel | Peripheral |
|---------|-----------|
| 0 | MDEC (video decoder) - input |
| 1 | MDEC - output |
| 2 | GPU |
| 3 | CDROM |
| 4 | SPU |
| 5 | PIO (parallel port) |
| 6 | OTC (Ordering Table Clear) |

Three transfer modes are supported:
- **Burst**: continuous single-block transfer
- **Slice**: transfer in chunks with CPU yielding between each chunk
- **Linked List**: traversal of a linked list in RAM (used for GPU display lists)

**Rationale**: DMA is essential for performance as it enables memory transfers without CPU involvement. The Linked List mode is PS1-specific and critical for 3D rendering (games build GPU command lists in RAM).

### 2.8 Interrupt Controller

The `InterruptController` manages two registers:
- **I_STAT**: status bits (set by peripherals, cleared by CPU writes)
- **I_MASK**: enable mask

Supported interrupt sources: VBLANK, GPU, CDROM, DMA, TIMER0-2, CONTROLLER_MEMCARD, SIO, SPU, LIGHTPEN.

When a peripheral raises an IRQ (`triggerIRQ(DeviceIRQ)`), the controller notifies the CPU via `m_bus->getCpu()->setInterruptPending(true)`. The CPU checks for pending interrupts at the beginning of every `step()`.

**Rationale**: This is the only exception to the mediator pattern (the signal propagates from the controller back to the CPU through the Bus). This choice is imposed by the hardware: interrupts must be handled before the next instruction.

### 2.9 GUI and Debugger Architecture

**Main loop** (`Application::run()`):
```
Initialization: GLFW + OpenGL 3.3 + ImGui
Load BIOS
Main loop:
  ├── glfwPollEvents()       # Window/input events
  ├── update()               # System::update(): runs ~565k CPU cycles per frame (33.868 MHz / 60 FPS)
  └── render()               # ImGui frame + all windows + VRAM upload -> OpenGL texture
```

**IWindow interface**: All debug windows implement a common interface:
```cpp
class IWindow {
    virtual void update() = 0;   // draws ImGui widgets
    void setVisible(bool);
    void toggleVisibility();
};
```

**Debugger hook**: The debugger is plugged into the emulation core via a callback:
```cpp
m_system.setDebuggerCallback([this]() { m_debugger.update(); });
```
This callback is invoked after every CPU instruction in `System::tick()`. If a breakpoint matches the current PC, emulation is paused.

**Rationale**:
- ImGui in immediate mode is ideal for a debugger since the interface changes every frame (registers, memory, disassembly)
- The callback decouples the emulation core (`rgmcore`) from the graphical interface
- ImGui docking allows the user to arrange windows as they see fit

### 2.10 Save State System

All components implement `serialize(StateBuffer &buf)` and `deserialize(StateBuffer &buf)`. The format is a flat binary blob prefixed with a magic number (`0x524F4745` = "ROGE") and a version number.

The serialization order is deterministic (CPU then Bus, which serializes its peripherals in a fixed order) despite the hash map storage.

**Rationale**: A flat binary format is the most efficient for fast save/load operations. The deterministic order ensures compatibility across save states.

---

## 3. Implementation Choices

### 3.1 Design Patterns Used

| Pattern | Where | Why |
|---------|-------|-----|
| **Mediator** | `Bus` | Decouples all peripherals from each other, single point of communication |
| **Template Method** | `PsxDevice` (abstract class) | Each peripheral implements the same virtual methods (`read8/16/32`, `write8/16/32`, `update`, `reset`) |
| **Type-Indexed Registry** | `Bus::m_devices` (`unordered_map<type_index, ...>`) | Type-safe service locator, O(1) access by C++ type |
| **Visitor (serialization)** | `StateBuffer` + `serialize/deserialize` | Flat-binary pattern for save states |
| **Callback / Lightweight Observer** | `System::setDebuggerCallback()` | Decouples the emulation core from the debugger and graphical interface |
| **Pimpl** | `Disc` (libcuebin) | Hides implementation details of CD reading |
| **Result type** | `Result<T>` (libcuebin) | Error handling without exceptions, based on `std::variant<T, Error>` |

**Patterns intentionally not used**:
- **No singleton**: `System` is stack-allocated in `Application`, no global state
- **No message queue**: interrupts are synchronous function calls (simpler, sufficient for instruction-by-instruction emulation)
- **No global state**: all state is owned by `System`, `Bus`, or their children

### 3.2 MIPS Instruction Pipeline

The execution cycle of a single instruction (`CPU::step()`):

```
1. fetchInstruction()      → loads the 32-bit word at the PC address from the Bus
2. Branch delay slot handling (tracked via m_inBranchDelay, m_nextIsBranchDelay)
3. Pending interrupt check → triggerException(Interrupt) if needed
4. executeInstruction()    → decodes the opcode (top 6 bits) and dispatches
5. handleLoadDelay()       → applies the delayed write from the load delay slot
6. checkTtyOutput()        → BIOS TTY hook for debugging
7. m_pc = m_nextPc         → advances the program counter
```

**Delay slots**:
- **Branch delay slot**: the instruction following a branch is always executed (MIPS specificity). Implemented via a `m_nextIsBranchDelay` flag that delays the branch effect by one cycle.
- **Load delay slot**: the result of a `load` is not available until the next cycle. Implemented via a 2-entry ring buffer (`m_loadDelaySlots[2]`).

**Rationale**: Instruction-by-instruction emulation (interpreter) is the simplest and most reliable choice for a PS1 emulator. A dynamic recompiler (JIT) would be more performant but considerably more complex to implement and debug.

### 3.3 GTE (Geometry Transform Engine)

The GTE is coprocessor 2 (COP2) of the MIPS. It performs 3D computations in fixed-point arithmetic:
- 32 data registers + 32 control registers
- 3-entry RGB FIFO for color operations
- FLAG register (control register 31) for overflow tracking

Implemented operations:
- **RTPS/RTPT**: perspective projection (3D -> 2D transformation)
- **NCLIP**: backface culling (back-face elimination)
- **MVMVA**: generic matrix-vector multiplication
- **NCDS/NCDT/NCCS/NCCT**: color and lighting calculations
- **AVSZ3/AVSZ4**: Z-depth averaging
- **OP**: cross product
- **SQR**: element-wise square
- **GPF/GPL**: generic interpolation
- **DPCS/DPCT/DCPL/INTPL/CDP**: depth-cue interpolation

**Rationale**: The GTE is indispensable for PS1 3D games. All 3D transformation, lighting, and projection calculations go through it. Without the GTE, no 3D game can function. The fixed-point implementation (rather than floating-point) faithfully reproduces the behavior of the original hardware, including precision artifacts (texture wobbling).

### 3.4 GPU Rasterization

The GPU implements software rasterization on the VRAM:
- **Polygons**: triangles and quads (textured or not, gouraud-shaded or flat)
- **Lines**: Bresenham algorithm
- **Rectangles**: fast fill
- **VRAM copies**: VRAM-VRAM, CPU-VRAM, VRAM-CPU
- **Texture sampling**: `sampleTexture(u, v, texInfo)`

GPU timing tracks NTSC scanlines (263 per frame, 3413 dot-cycles per line) and triggers a VBLANK IRQ at the end of each frame.

The VRAM is then uploaded as an OpenGL texture (`GL_UNSIGNED_SHORT_1_5_5_5_REV`, 1024x512) via `glTexSubImage2D` for display.

**Rationale**: Software rasterization is the faithful choice for emulating the PS1 GPU. The original hardware performs no bilinear filtering or antialiasing; hardware rasterization (modern GPU) would introduce visual differences. The OpenGL upload is minimal and does not impact performance.

### 3.5 Input Handling (Controller)

The input communication chain reproduces the PS1 serial protocol:

```
Application::pollGamepad()  →  System::updatePadInputs()  →  SerialInterface::getPad(0).updateButtons()
         ↑ GLFW input                                                      │
                                                                 ┌─────────▼─────────┐
                                                                 │    DigitalPad      │
                                                                 │  5-state protocol  │
                                                                 │ HighZ → IDLO →     │
                                                                 │ IDHI → MOT0 → MOT1 │
                                                                 └───────────────────┘
```

The `DigitalPad` implements a 5-state automaton that reproduces the real serial handshake between the console and the controller. `SIO0` manages two controller slots. `SerialInterface` exposes everything as a `PsxDevice` on the Bus.

**Rationale**: Reproducing the real serial protocol (rather than simple button mapping) is necessary because some games test the protocol directly and expect specific responses in the correct order.

### 3.6 libcuebin: CD-ROM Library

Two-layer architecture:

**Layer 1 - CUE Parsing**:
- `CueParser::parseFile()` / `parseString()` returns a `Result<CueSheet>`
- `CueSheet` contains files, tracks, indices, metadata
- Supported modes: Audio, Mode1/2048, Mode1/2352, Mode2/2336, Mode2/2352

**Layer 2 - Disc Abstraction**:
- `Disc::fromCue(path)` returns a `Result<Disc>`
- API: `readSector(lba)`, `readSectors(lba, count)`, `findTrack(lba)`, `leadOutLba()`
- Pimpl pattern to hide implementation details
- Lazy file opening with per-file mutex for concurrent access

**Error handling**: `Result<T>` (based on `std::variant<T, Error>`) instead of exceptions, with `LIBCUEBIN_TRY(expr)` macro for propagation.

**Rationale**:
- The `Result<T>` approach is more explicit than exceptions: the caller is forced to handle the error
- The Pimpl pattern allows modifying the internal implementation without recompiling files that use `Disc`
- Lazy opening and mutexes enable thread-safe usage without startup performance penalties

### 3.7 Testing Strategy

**Framework**: Google Test, linked against the `rgmcore` static library.

**Fixture-based approach**: Most CPU tests use fixtures that instantiate a real `Bus` + `CPU`:

```cpp
class CpuArithmeticTest : public testing::Test {
    Bus bus;
    CPU cpu;
    CpuArithmeticTest() : cpu(&bus) { cpu.setReg(CpuReg::PC, 0x10000); }
};
```

Tests write raw instruction words directly into RAM, call `cpu.step()`, and assert on register values. This tests the full decode-execute-writeback path without mocking.

**GTE tests**: The GTE is instantiated directly without a Bus (it does not inherit from `PsxDevice`). Registers are manipulated via `ctc()`, `mtc()`, `mfc()`, then `execute(opcode)`.

**Test coverage**:

| Component | Test Files | Coverage |
|-----------|-----------|----------|
| CPU (arithmetic, branches, comparisons, jumps, load/store, shifts, logic, mul/div) | 12 files | Complete |
| CPU (COP0, exceptions) | 2 files | Complete |
| GTE (coordinates, colors, calculations, instructions, projections) | 7 files | High |
| GPU (registers) | 1 file | Basic |
| Bus, DMA, Timers, InterruptController, RAM, BIOS, ScratchPad, SPU, etc. | Integrated | Variable |

**Rationale**:
- Testing with real components (`Bus` + `CPU`) rather than mocks ensures that interactions are actually validated
- GTE isolation (without Bus) enables fast, targeted tests on mathematical operations
- Coverage priority is given to the CPU and GTE as they are the most critical components and most prone to precision bugs
