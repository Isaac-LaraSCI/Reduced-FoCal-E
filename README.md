## 2026 Cal Poly Senior Project

# Introduction
This repository contains a Geant4 simulation of a prototype FoCal-E calorimeter model for the ALICE experiment at CERN. The structure consists of an aluminum frame encasing a multilayer sampling calorimeter with Tungsten absorber layers and Silicon detector layers with differing levels of granularity. A Gaussian-profile particle beam was modeled to direct electrons with energies from 0.5 MeV to 287 GeV toward the FoCal-E. The resulting energy deposition from the induced electromagnetic showers within the detector layers was then analyzed. The results of this simulation are published in my senior project thesis at Cal Poly, San Luis Obispo.

# Disclaimer
This code represents my first Geant4 project. As a result, I outlined the code’s limitations in the ‘Limitations’ section of this document. The implementation drew inspiration from Geant4 Basic Example B4. AI assistance was used to clarify some information/techniques learned through online documentation and resources. 

As for the accompanying ROOT files, AI assistance was also used to adapt the longitudinal and transverse energy distribution macros from a single-case visualization to a multi-case comparison layout for brevity.

# Build Requirements  
- CMake 3.16 or newer
- A Geant4 installation built with UI and visualization support
- A C++ compiler compatible with the installed Geant4 version
- ROOT for analyzing the generated outputted files (RECOMMENDED)

# Installation
There are plenty of tutorials online showcasing the Geant4 installation process for Windows, Mac, and Linux. Good news, most of this directory is entirely portable across all of these platforms. All you need to do is simply change some path variables that I will point out later. One thing to note is that my visualization macros in the ‘Macros’ directory feature OGLIQt, which is available on Linux when Geant4 is built with Qt and OpenGL support. Bad news, getting OGLIQt to work on my computer was an absolute nightmare spanning multiple days of work so I recommend taking a look at the Geant4 Forum for assistance. You’re smart, I have faith in you!

Below are sources that were very helpful for installing Geant (aside from LLMs).
- https://geant4.web.cern.ch/documentation/dev/ig_html/InstallationGuide/
- https://geant4-forum.web.cern.ch/
- https://www.youtube.com/watch?v=Lxb4WZyKeCE (LINUX)

Included in this repository is a shell script, run.sh, which configures, builds, and runs the Geant4 simulation. Before its first use, grant the script execute permission with the one-time command chmod +x run.sh. The simulation can then be built and run with ./run.sh.

# Limitations

- Event Simulation
The current electron beam code is currently configured to run through the Geant4 UI’s command terminal. The results obtained in my senior project were limited to events in the order of thousands due to my current computer’s technical limitations. To increase the number of events, all you need to do is run the following line:

Command: /run/beamOn [NUMBER OF EVENTS YOU WANT]

- Beam Construction
The electron-beam configuration is currently hard-coded in “PrimaryGeneratorAction.hh”, located in the “include/” directory. This includes the particle type, beam energy, number of particles per event, initial position, propagation direction, and Gaussian transverse widths.
While this approach is adequate for a small number of test configurations, repeatedly recompiling the project to change beam parameters can become inconvenient. I recommend moving the beam configuration into a Geant4 macro file. This would allow users to adjust parameters such as particle energy, beam width, and number of particles per event directly from the Geant4 command interface, without modifying and recompiling the source code.

- Seed Handling
The simulation uses Geant4’s random-number engine for the Gaussian beam-position sampling, but it does not explicitly set and record random-number seeds. As a result, reproducing a particular run exactly is practically impossible for the current set up. A future, vital improvement would be to set the seed values at the beginning of each run through a macro and save those values alongside the corresponding ROOT output file. The importance of reproducible seed handling was reinforced during my 2026 summer internship at Lawrence Livermore National Laboratory.

# CUSTOMIZATION

This repository models a fixed 40-layer tungsten–silicon calorimeter configuration. If you wish to fundamentally change the configuration of the absorber and sensor layers, you may have to alter DetectorConstruction.cc in the src directory significantly. Embedded within the source file are outlined sections that explain what is happening underneath the hood.

If you wish to keep the general configuration but make minor tweaks, below is a list of variables you can readily change.

- **Src/DetectorConstruction.cc**
checkOverlaps: This allows you to toggle Geant4’s geometry-overlap checker. This is excellent for checking whether any volumes are overlapping with one another.
globe_size: This defines the size of the world volume containing the system. Since the overall system is not particularly large, you probably will not need to change this.
absHx and senHx: These define the half-thicknesses of the absorber and sensor layers along the beam direction. The overall longitudinal length of the detector is defined using these parameters, so changing them will adjust the total depth of the calorimeter while preserving its general configuration.
frameVis, absLV, hgsLV, and lgsLV color values: These define the colors of the frame, absorber, high-granularity sensors, and low-granularity sensors in the visualizer window. Changing these values does not affect the physics of the simulation.
- **Src/RunAction.cc**
My method creates files by recording the time the run was executed. In retrospect, it would have been better to simply use loops to name files. Change as you see fit!  
This code outputs .root files.
- **Src/PrimaryGeneratorAction.cc**
particle: Defines the particle species used by the particle gun.
fBeamPos: Defines the beam position of the particle gun.
fGun->SetParticleMomentumDirection(...): Defines the direction of the particle beam.
Include/PrimaryGeneratorAction.hh
E: Defines the energy of the particle gun. The default is set in units of MeV.
N: Defines the number of particles used in the beam.
fSigmaY and fSigmaZ: Define the Gaussian width of the beam in the transverse directions. The default is set in units of mm.


# CONFIGURATION

Config.cc creates and initializes the Geant4 run manager and calorimeter environment. It also loads up the Qt visualization session. During startup, it executes init_vis.mac, which opens the OpenGL Qt viewer, draws the detector volume in wireframe mode, and sets its initial viewing angle. It then executes gui.mac, which adds the custom controls to the Qt interface. For these files, many different lines were written to suppress certain outputs and configure the application’s startup and visualization behavior.
