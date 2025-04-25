### **BETA TEST PLAN – ROGEM**

## **1. Core Functionalities for Beta Version**
Below are the essential features that must be available for beta testing, along with any changes made since the initial Tech3 Action Plan.

| **Feature Name**                    | **Description**                                                                                  | **Priority (High/Medium/Low)** | **Changes Since Tech3**         |
| ----------------------------------- | ------------------------------------------------------------------------------------------------ | ------------------------------ | ------------------------------- |
| Emulate main Playstation components | CPU interpreter<br>GPU<br>SPU (at best)<br>Memory                                                | High                           | -                               |
| Emulate a game                      | Run a game with no or exceptional crashes or slow-downs at worst                                 | High                           | Run fewer games                 |
| Debugger                            | - Visualize internal system state<br>- Add/remove breakpoints<br>- Control the system externally | High                           | -                               |
| Cross platform compatibility        | The emulator must be available for Linux/Windows                                                 | Medium                         | Remove mobile and MacOS support |
| Keybind customization               | Allow the user to set his own keybindings                                                        | Medium                         | -                               |
| Controller support                  | Allow the user to play using an XBOX Series controller                                           | Medium                         | Change controller type          |
| Sound                               | Minimal sound emulation                                                                          | Low                            | -                               |

---

## **2. Beta Testing Scenarios**
### **2.1 User Roles**

| **Role Name** | **Description**                                         |
| ------------- | ------------------------------------------------------- |
| Developer     | Use the debugger, knows the codebase, test the emulator |
| User          | Play games, use the debugger, change user settings      |

### **2.2 Test Scenarios**
For each core functionality, provide detailed test scenarios.

#### **Scenario 1: Emulate main Playstation components**
- **Role Involved:** Developer
- **Objective:** Make sure the system behaves as close as possible to the real hardware
- **Preconditions:** Gather PSX test binaries
- **Test Steps:**
  1. Build the project
  2. Run unit tests
  3. Run Amidog's CPU PSX-EXE tests
- **Expected Outcome:** Every test passes

#### **Scenario 2: Emulate a game**
- **Role Involved:** Developer/User
- **Objective:** The emulator can run the game smoothly
- **Preconditions:** Have a working PSX BIOS and a "Crash Bandicoot " ROM
- **Test Steps:**
  1. Set the BIOS file in the emulator
  2. Select the game you want to run
  3. Run the game
  4. Start a new game
  5. Play the first level of the game
  6. Jump on a crate
  7. Gather mangoes
  8. Break a crate
  9. Kill enemies while spinning the character
  10. Reach a checkpoint
  11. Die and respawn on a checkpoint
  12. Finish the level
- **Expected Outcome:** The game runs smoothly, is playable, very few unnoticeable slow-downs at worst and graphically accurate

#### **Scenario 3: Debugger**
- **Role Involved:** Developer/User
- **Objective:** Visualize and control the system states
- **Preconditions:** Have a working PSX BIOS (game ROMs are optional)
- **Test Steps:**
 1. Launch the emulator
 2. Pause execution at BIOS entry
 3. Open the debugger windows
 4. Step through the code by hand
 5. Check expected results after each step

- **Expected Outcome:** The expected results are displayed in the correct debugger windows

#### **Scenario 4: Cross platform compatibility**
- **Role Involved:** Developer/User
- **Objective:** The emulator must run on Windows and Linux
- **Preconditions:** Have a working PSX BIOS and game ROMs
- **Test Steps:**
 1. Build the emulator on Windows
 2. Launch the emulator
 3. Play a game
 4. Build the emulator on Linux
 5. Launch the emulator on Linux
 6. Play game

- **Expected Outcome:** Games are playable both on Windows and Linux

#### **Scenario 5: Controller support / keybind customization**
- **Role Involved:** Developer/User
- **Objective:** The emulator must run on Windows and Linux
- **Preconditions:** Have a working PSX BIOS and game ROMs
- **Test Steps:**
 1. Launch the emulator
 2. Open key settings
 3. Change settings to custom bindings
 4. Play a game

- **Expected Outcome:** The game is playable and the the keybindings are properly mapped


---

## **3. Success Criteria**
The following criteria will be used to determine the success of the beta version.

| **Criterion** | **Description**                                                                                | **Threshold for Success**             |
| ------------- | ---------------------------------------------------------------------------------------------- | ------------------------------------- |
| Stability     | No major crashes or critical bugs                                                              | No crash reported under usual usage   |
| Usability     | Users can navigate and understand features with minimal guidance.<br>Intuitive UI for the user | 80% positive feedback from testers    |
| Performance   | A game from the 5 games list runs smoothly and is playable                                     | Runs at least at 80% of the framerate |

---

## **4. Known Issues & Limitations**

| **Issue**                  | **Description**                                                                                 | **Impact** | **Planned Fix? (Yes/No)** |
| -------------------------- | ----------------------------------------------------------------------------------------------- | ---------- | ------------------------- |
| Only 5 games can be played | Some games use uncommon features and tricks to be able to run, which makes them hard to emulate | High       | Yes, after beta           |
| No saving                  | Missing save-states and memory card emulation                                                   | High       | Yes, after beta           |
| Sound                      | Sound emulation is not accurate                                                                 | Medium     | Yes, after beta           |

---

## **5. Conclusion**
The **Beta Test Plan** allows us to validate core functionalities and uncover possible bugs.
We focus on stability, performance, and usability, this beta phase ensures the emulator can play a small set of games. This beta phase will help us improve the emulator by finding issues and think of new features to prepare for the first release version of the project, including better compatibility, save functionality, and enhanced audio.
