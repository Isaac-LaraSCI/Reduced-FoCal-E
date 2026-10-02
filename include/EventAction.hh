#pragma once

#include "G4UserEventAction.hh"
#include "G4Types.hh"

#include <map>
#include <tuple>

class G4Event;
class RunAction;

class EventAction : public G4UserEventAction
{
public:
  explicit EventAction(RunAction* runAction);
  ~EventAction() override = default;

  void BeginOfEventAction(const G4Event*) override;
  void EndOfEventAction(const G4Event*) override;

  void AddEdep(G4double edep) { fEdep += edep; }

  void AddEdepCell(G4int layerMarker, G4int row, G4int col, G4double edep)
  {
    fEdepByCell[std::make_tuple(layerMarker, row, col)] += edep;
  }

private:
  RunAction* fRunAction = nullptr;

  G4double fEdep = 0.;

  std::map<std::tuple<G4int, G4int, G4int>, G4double> fEdepByCell;

};