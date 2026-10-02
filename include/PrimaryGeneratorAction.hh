#pragma once

#include <string>

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"

class G4Event;
class G4ParticleGun;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  PrimaryGeneratorAction();
  ~PrimaryGeneratorAction() override;

  void GeneratePrimaries(G4Event* evt) override;

  // BEAM CONTROL CENTER

  int N = 1;
  G4double E = 287000.0*MeV;

  G4double fSigmaY = 0.5*mm;
  G4double fSigmaZ = 0.5*mm;

private:
  G4ParticleGun* fGun = nullptr;

  G4ThreeVector fBeamPos;
};