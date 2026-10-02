#include "RunAction.hh"
#include "PrimaryGeneratorAction.hh"

#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include "G4SystemOfUnits.hh"

#include <ctime>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <string>

RunAction::RunAction()
: G4UserRunAction()
{
  auto analysisManager = G4AnalysisManager::Instance();

  // fFileName START

  // SECTION IS ENTIRELY DEPENDENT ON YOUR OWN FILE PATHS
  // FILE NAMES ARE GIVEN BY CHRONO LIBRARY
  auto now = std::chrono::system_clock::now();
  std::time_t now_time = std::chrono::system_clock::to_time_t(now);

  std::tm tm_buf;
  localtime_r(&now_time, &tm_buf);

  std::ostringstream timeStream;
  timeStream << std::put_time(&tm_buf, "%Y-%m-%d_%H-%M-%S");

  G4int runNumber = 1000;
  G4int eNumber   = 20;

  fFileName = "/home/ilara05/projects/FoCalESimulation/files/"
            + std::to_string(runNumber)
            + "_RUNS_"
            + std::to_string(eNumber)
            + "_ENERGY_GeV_"
            + timeStream.str()
            + "_DATE";

  // fFileName FINISH

  analysisManager->SetFileName(fFileName);
  analysisManager->SetDefaultFileType("root"); // ROOT FILE OUTPUT
  analysisManager->SetNtupleMerging(true);

  // Ntuple 0 [TOTAL EVENT ENERGY]
  analysisManager->CreateNtuple("Event", "Total Energy of Event");
  analysisManager->CreateNtupleIColumn("eventID");
  analysisManager->CreateNtupleDColumn("totalEdep_MeV");
  analysisManager->FinishNtuple();

  // Ntuple 1 [ENERGY PER CELL]
  analysisManager->CreateNtuple("Cell", "Energy per cell");
  analysisManager->CreateNtupleIColumn("eventID");
  analysisManager->CreateNtupleIColumn("Layer_Marker");
  analysisManager->CreateNtupleIColumn("row");
  analysisManager->CreateNtupleIColumn("col");
  analysisManager->CreateNtupleDColumn("cellEdep_MeV");
  analysisManager->FinishNtuple();
}

RunAction::~RunAction()
{
  delete G4AnalysisManager::Instance();
}

void RunAction::BeginOfRunAction(const G4Run*)
{
  G4AnalysisManager::Instance()->OpenFile(fFileName);
}

void RunAction::EndOfRunAction(const G4Run*)
{
  auto analysisManager = G4AnalysisManager::Instance();

  analysisManager->Write();
  analysisManager->CloseFile();
}

void RunAction::FillEvent(G4int Event_Number, G4double Total_Edep)
{
  auto analysisManager = G4AnalysisManager::Instance();

  analysisManager->FillNtupleIColumn(0, 0, Event_Number);
  analysisManager->FillNtupleDColumn(0, 1, Total_Edep/MeV);
  analysisManager->AddNtupleRow(0);
}

void RunAction::FillCell(G4int Event_Number,
                         G4int layerMarker,
                         G4int row,
                         G4int col,
                         G4double Edep)
{
  auto analysisManager = G4AnalysisManager::Instance();

  analysisManager->FillNtupleIColumn(1, 0, Event_Number);
  analysisManager->FillNtupleIColumn(1, 1, layerMarker);
  analysisManager->FillNtupleIColumn(1, 2, row);
  analysisManager->FillNtupleIColumn(1, 3, col);
  analysisManager->FillNtupleDColumn(1, 4, Edep/MeV);
  analysisManager->AddNtupleRow(1);
}