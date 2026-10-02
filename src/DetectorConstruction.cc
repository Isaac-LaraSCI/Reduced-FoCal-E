#include "DetectorConstruction.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include <vector>
#include <string>

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  G4bool checkOverlaps = false;

  // FRAME DIMENSIONS
  const G4double globe_size = 300.0*cm;
  const G4double frameHy = 1.1*cm;
  const G4double frameHz = 1.1*cm;
  const G4double barT = 0.05*cm;

  // CORE X HALF-THICKNESSES
  const G4double absHx = 0.175*cm;
  const G4double senHx = 0.016*cm;

  // EXACT CORE STACK LENGTH:
  const G4double stackLength = 20*(2.0*absHx) + 20*(2.0*senHx);
  const G4double stackHalfX  = stackLength/2.0;

  const G4double frameHx = stackHalfX + 2.0*barT;

  // MATERIALS
  auto nist = G4NistManager::Instance();
  auto Vacuum   = nist->FindOrBuildMaterial("G4_Galactic");
  auto Aluminum = nist->FindOrBuildMaterial("G4_Al");
  auto Silicon  = nist->FindOrBuildMaterial("G4_Si");

  G4Element* W  = nist->FindOrBuildElement("W");
  G4Element* Ni = nist->FindOrBuildElement("Ni");
  G4Element* Cu = nist->FindOrBuildElement("Cu");

  G4Material* Tungsten_Alloy = new G4Material("Tungsten_Alloy", 17.8*g/cm3, 3);
  Tungsten_Alloy->AddElement(W,  0.94);
  Tungsten_Alloy->AddElement(Ni, 0.04);
  Tungsten_Alloy->AddElement(Cu, 0.02);

  // WORLD
  auto worldSolid = new G4Box("World", globe_size, globe_size, globe_size);
  auto worldLV    = new G4LogicalVolume(worldSolid, Vacuum, "WorldLV");
  auto worldPV    = new G4PVPlacement(nullptr, {}, worldLV, "WorldPV", nullptr, false, 0);

  // FRAME
  auto edgeX_s = new G4Box("edgeX", frameHx, barT, barT);
  auto edgeY_s = new G4Box("edgeY", barT, frameHy - 0.1*cm, barT);
  auto edgeZ_s = new G4Box("edgeZ", barT, barT, frameHz - 0.1*cm);

  auto edgeX_lv = new G4LogicalVolume(edgeX_s, Aluminum, "edgeX_lv");
  auto edgeY_lv = new G4LogicalVolume(edgeY_s, Aluminum, "edgeY_lv");
  auto edgeZ_lv = new G4LogicalVolume(edgeZ_s, Aluminum, "edgeZ_lv");

  auto frameVis = new G4VisAttributes(G4Colour(1.0, 0.0, 1.0));

  edgeX_lv->SetVisAttributes(frameVis);
  edgeY_lv->SetVisAttributes(frameVis);
  edgeZ_lv->SetVisAttributes(frameVis);

  const G4double xE = frameHx - barT;
  const G4double yE = frameHy - barT;
  const G4double zE = frameHz - barT;

  // EDGES
  new G4PVPlacement(nullptr, {0,  yE,  zE}, edgeX_lv, "edgeX", worldLV, false, 1, checkOverlaps);
  new G4PVPlacement(nullptr, {0,  yE, -zE}, edgeX_lv, "edgeX", worldLV, false, 2, checkOverlaps);
  new G4PVPlacement(nullptr, {0, -yE,  zE}, edgeX_lv, "edgeX", worldLV, false, 3, checkOverlaps);
  new G4PVPlacement(nullptr, {0, -yE, -zE}, edgeX_lv, "edgeX", worldLV, false, 4, checkOverlaps);

  new G4PVPlacement(nullptr, { xE, 0,  zE}, edgeY_lv, "edgeY", worldLV, false, 1, checkOverlaps);
  new G4PVPlacement(nullptr, { xE, 0, -zE}, edgeY_lv, "edgeY", worldLV, false, 2, checkOverlaps);
  new G4PVPlacement(nullptr, {-xE, 0,  zE}, edgeY_lv, "edgeY", worldLV, false, 3, checkOverlaps);
  new G4PVPlacement(nullptr, {-xE, 0, -zE}, edgeY_lv, "edgeY", worldLV, false, 4, checkOverlaps);

  new G4PVPlacement(nullptr, { xE,  yE, 0}, edgeZ_lv, "edgeZ", worldLV, false, 1, checkOverlaps);
  new G4PVPlacement(nullptr, { xE, -yE, 0}, edgeZ_lv, "edgeZ", worldLV, false, 2, checkOverlaps);
  new G4PVPlacement(nullptr, {-xE,  yE, 0}, edgeZ_lv, "edgeZ", worldLV, false, 3, checkOverlaps);
  new G4PVPlacement(nullptr, {-xE, -yE, 0}, edgeZ_lv, "edgeZ", worldLV, false, 4, checkOverlaps);

  // CORE
  const G4double coreHy = (frameHy - 2.0*barT)/2.0;
  const G4double coreHz = (frameHz - 2.0*barT)/2.0;

  const G4double coreHy_new = 0.005*cm;
  const G4double coreHz_new = 0.005*cm;

  auto Absorbers  = new G4Box("Abs_Core", absHx, 2.0*coreHy, 2.0*coreHz);
  auto HG_Sensors = new G4Box("HGS_Core", senHx, coreHy_new, coreHz_new);
  auto LG_Sensors = new G4Box("LGS_Core", senHx, coreHy, coreHz);

  auto absLV = new G4LogicalVolume(Absorbers,  Tungsten_Alloy, "Abs_Core");
  auto hgsLV = new G4LogicalVolume(HG_Sensors, Silicon, "HGS_Core");
  auto lgsLV = new G4LogicalVolume(LG_Sensors, Silicon, "LGS_Core");

  fAbs = absLV;
  fHGS = hgsLV;
  fLGS = lgsLV;

  absLV->SetVisAttributes(new G4VisAttributes(G4Colour(0.0, 0.0, 1.0)));
  hgsLV->SetVisAttributes(new G4VisAttributes(G4Colour(0.0, 1.0, 0.0)));
  lgsLV->SetVisAttributes(new G4VisAttributes(G4Colour(1.0, 0.0, 0.0)));


  // PLACEMENT AND INDEXING

  // NOTE:
  // ARRAYS ARE INDEXED BY creation_id/copy number.
  // fCreationArr[copyNo] = layer number
  // fRowArr[copyNo]      = row index
  // fColArr[copyNo]      = column index

  fCreationArr.clear();
  fRowArr.clear();
  fColArr.clear();

  int creation_id = 0;

  // NOTE: FULL STACK SPANS EXACTLY X = -stackHalfX to +stackHalfX
  G4double xLeft = -stackHalfX;

  for (int i = 0; i < 40; i++) {

    const G4bool isHG = (i == 9 || i == 19);
    const G4bool isLG = (!isHG && (i % 2 != 0));

    const G4double hx = (isHG || isLG) ? senHx : absHx;
    const G4double x  = xLeft + hx;

    if (isHG) {

      G4double init = 0.995*cm;

      for (int row = 0; row < 200; row++) {
        for (int col = 0; col < 200; col++) {

          new G4PVPlacement(
            nullptr,
            {x, 0.01*row*cm - init, 0.01*col*cm - init},
            hgsLV,
            "HGS_Core",
            worldLV,
            false,
            creation_id,
            checkOverlaps
          );

          fCreationArr.push_back(i);
          fRowArr.push_back(row);
          fColArr.push_back(col);

          creation_id++;
        }
      }
    }

    else if (isLG) {

      new G4PVPlacement(
        nullptr,
        {x, coreHy, coreHz},
        lgsLV,
        "LGS_Core",
        worldLV,
        false,
        creation_id,
        checkOverlaps
      );

      fCreationArr.push_back(i);
      fRowArr.push_back(0);
      fColArr.push_back(0);
      creation_id++;

      new G4PVPlacement(
        nullptr,
        {x, -coreHy, -coreHz},
        lgsLV,
        "LGS_Core",
        worldLV,
        false,
        creation_id,
        checkOverlaps
      );

      fCreationArr.push_back(i);
      fRowArr.push_back(1);
      fColArr.push_back(1);
      creation_id++;

      new G4PVPlacement(
        nullptr,
        {x, -coreHy, coreHz},
        lgsLV,
        "LGS_Core",
        worldLV,
        false,
        creation_id,
        checkOverlaps
      );

      fCreationArr.push_back(i);
      fRowArr.push_back(1);
      fColArr.push_back(0);
      creation_id++;

      new G4PVPlacement(
        nullptr,
        {x, coreHy, -coreHz},
        lgsLV,
        "LGS_Core",
        worldLV,
        false,
        creation_id,
        checkOverlaps
      );

      fCreationArr.push_back(i);
      fRowArr.push_back(0);
      fColArr.push_back(1);
      creation_id++;
    }

    else {

      new G4PVPlacement(
        nullptr,
        {x, 0, 0},
        absLV,
        "Abs_Core",
        worldLV,
        false,
        creation_id,
        checkOverlaps
      );

      fCreationArr.push_back(i);
      fRowArr.push_back(0);
      fColArr.push_back(0);

      creation_id++;
    }

    xLeft += 2.0*hx;
  }

  worldLV->SetVisAttributes(G4VisAttributes::GetInvisible());

  return worldPV;
}
const std::vector<int>& DetectorConstruction::GetCreationArr() const {
  return fCreationArr;
}
const std::vector<int>& DetectorConstruction::GetRowArr() const {
  return fRowArr;
}
const std::vector<int>& DetectorConstruction::GetColArr() const {
  return fColArr;
}