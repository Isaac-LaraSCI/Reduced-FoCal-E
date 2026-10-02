#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class EventAction;
class G4LogicalVolume;
class G4Step;

class SteppingAction : public G4UserSteppingAction
{
public:
  SteppingAction(EventAction* eventAction);
  ~SteppingAction() override = default;

  void UserSteppingAction(const G4Step* step) override;

private:
  EventAction* fEventAction = nullptr;

  G4LogicalVolume* fAbsLV = nullptr;
  G4LogicalVolume* fHGSLV = nullptr;
  G4LogicalVolume* fLGSLV = nullptr;
};

#endif