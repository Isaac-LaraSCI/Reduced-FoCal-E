#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"

#include "FTFP_BERT.hh"
#include "G4EmParameters.hh"
#include "G4HadronicProcessStore.hh"
#include "G4HadronicParameters.hh"

int main(int argc, char** argv)
{
  auto ui = new G4UIExecutive(argc, argv);

  auto* runManager =
    G4RunManagerFactory::CreateRunManager(G4RunManagerType::MT);

  runManager->SetVerboseLevel(0);
  runManager->SetNumberOfThreads(1);

  runManager->SetUserInitialization(new DetectorConstruction());
  runManager->SetUserInitialization(new FTFP_BERT(0));
  runManager->SetUserInitialization(new ActionInitialization());

  G4EmParameters::Instance()->SetVerbose(0);
  G4HadronicProcessStore::Instance()->SetVerbose(0);
  G4HadronicParameters::Instance()->SetVerboseLevel(0);

  auto* UImanager = G4UImanager::GetUIpointer();

  runManager->Initialize();

  auto* visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();

  UImanager->ApplyCommand("/control/verbose 0");
  UImanager->ApplyCommand("/run/verbose 0");
  UImanager->ApplyCommand("/event/verbose 0");
  UImanager->ApplyCommand("/tracking/verbose 0");
  UImanager->ApplyCommand("/run/printProgress 0");

  UImanager->ApplyCommand("/control/execute ../macros/init_vis.mac");
  UImanager->ApplyCommand("/control/execute ../macros/gui.mac");

  ui->SessionStart();

  delete visManager;
  delete runManager;
  delete ui;

  return 0;
}