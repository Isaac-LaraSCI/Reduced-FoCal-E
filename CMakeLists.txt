cmake_minimum_required(VERSION 3.16)
project(FoCalESimulation)

find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(FoCalESimulation
  src/Config.cc
  src/DetectorConstruction.cc
  src/ActionInitialization.cc
  src/PrimaryGeneratorAction.cc
  src/EventAction.cc
  src/RunAction.cc
  src/SteppingAction.cc
  )

target_link_libraries(FoCalESimulation ${Geant4_LIBRARIES})

file(COPY ${PROJECT_SOURCE_DIR}/macros DESTINATION ${PROJECT_BINARY_DIR})