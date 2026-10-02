#pragma once

#include "G4VUserDetectorConstruction.hh"
#include <vector>

class G4VPhysicalVolume;
class G4LogicalVolume;

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
  G4VPhysicalVolume*Construct() override;

  G4LogicalVolume*Abs() const { return fAbs; }
  G4LogicalVolume*HGS() const { return fHGS; }
  G4LogicalVolume*LGS() const { return fLGS; }

  const std::vector<int>& GetCreationArr() const;
  const std::vector<int>& GetRowArr() const;
  const std::vector<int>& GetColArr() const;
  
private:
  G4LogicalVolume* fAbs = nullptr;
  G4LogicalVolume* fHGS = nullptr;
  G4LogicalVolume* fLGS = nullptr;

  std::vector<int> fCreationArr;
  std::vector<int> fRowArr;
  std::vector<int> fColArr;
};