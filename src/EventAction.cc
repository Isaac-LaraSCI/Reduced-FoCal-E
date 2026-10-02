#include "EventAction.hh"
#include "RunAction.hh"

#include "G4Event.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4ios.hh"

EventAction::EventAction(RunAction* runAction) : G4UserEventAction(), fRunAction(runAction)
{
}

void EventAction::BeginOfEventAction(const G4Event*)
{
  fEdep = 0.0;
  fEdepByCell.clear();
}

void EventAction::EndOfEventAction(const G4Event* event)
{
  const G4int eventID = event->GetEventID();

  G4cout << "Event " << eventID << ": Total Energy Deposition = " << fEdep / MeV << " MeV" << G4endl;

  fRunAction->FillEvent(eventID, fEdep);

  for (const auto& kv : fEdepByCell) {
    if (kv.second <= 0.) continue;

    const auto& key = kv.first;

    G4int layerMarker = std::get<0>(key);
    G4int row         = std::get<1>(key);
    G4int col         = std::get<2>(key);

    fRunAction->FillCell(eventID, layerMarker, row, col, kv.second);
  }
}