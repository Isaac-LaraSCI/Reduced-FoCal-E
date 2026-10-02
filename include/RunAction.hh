#pragma once

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;

class RunAction : public G4UserRunAction
{
public:
  RunAction();
  ~RunAction() override;

  void BeginOfRunAction(const G4Run* run) override;
  void EndOfRunAction(const G4Run* run) override;

  void FillEvent(G4int eventID, G4double totalEdep);
  void FillCell(G4int eventID, G4int layerMarker, G4int row, G4int col, G4double edep);
  
private:
  G4String fFileName;
};