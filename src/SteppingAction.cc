#include "SteppingAction.hh"

#include "EventAction.hh"
#include "DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4TouchableHandle.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"

SteppingAction::SteppingAction(EventAction* eventAction)
: G4UserSteppingAction(),
  fEventAction(eventAction)
{
}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
  if (!step || !fEventAction) return;

  const auto detConstruction =
    static_cast<const DetectorConstruction*>(
      G4RunManager::GetRunManager()->GetUserDetectorConstruction());

  if (!detConstruction) return;

  if (!fAbsLV || !fHGSLV || !fLGSLV) {
    fAbsLV  = detConstruction->Abs();
    fHGSLV  = detConstruction->HGS();
    fLGSLV  = detConstruction->LGS();
  }

  const auto prePoint = step->GetPreStepPoint();
  if (!prePoint) return;

  const auto touch = prePoint->GetTouchableHandle();
  const auto pv = touch->GetVolume();
  if (!pv) return;

  const auto lv = pv->GetLogicalVolume();

  if (lv != fHGSLV && lv != fLGSLV) return;

  const auto edep = step->GetTotalEnergyDeposit();
  if (edep <= 0.) return;

  fEventAction->AddEdep(edep);

  const G4int copyNo = pv->GetCopyNo();

  G4int layerMarker = -1;
  G4int row         = -1;
  G4int col         = -1;

  const auto& creationArr = detConstruction->GetCreationArr();
  const auto& rowArr      = detConstruction->GetRowArr();
  const auto& colArr      = detConstruction->GetColArr();

  if (copyNo >= 0 && copyNo < static_cast<G4int>(creationArr.size())) {
    layerMarker = creationArr[copyNo];
  }

  if (copyNo >= 0 && copyNo < static_cast<G4int>(rowArr.size())) {
    row = rowArr[copyNo];
  }

  if (copyNo >= 0 && copyNo < static_cast<G4int>(colArr.size())) {
    col = colArr[copyNo];
  }

  fEventAction->AddEdepCell(layerMarker, row, col, edep);
}