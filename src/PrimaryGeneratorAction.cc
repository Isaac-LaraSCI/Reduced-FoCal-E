#include "PrimaryGeneratorAction.hh"

#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4Event.hh"
#include "Randomize.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
  // BEAM CONSTRUCTION CODE
  auto particle = G4ParticleTable::GetParticleTable()->FindParticle("e-");

  fGun = new G4ParticleGun(N);
  fGun->SetParticleDefinition(particle);

  // BEAM CHARACTERIZATION CODE
  fGun->SetParticleEnergy(E);
  fGun->SetParticleMomentumDirection(G4ThreeVector(1, 0, 0));

  fBeamPos = G4ThreeVector((-3.82-10)*cm, 0.0, 0.0);
  fGun->SetParticlePosition(fBeamPos);

}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* evt)
{
  const G4double y = G4RandGauss::shoot(fBeamPos.y(), fSigmaY); 
  const G4double z = G4RandGauss::shoot(fBeamPos.z(), fSigmaZ);

  fGun->SetParticlePosition(G4ThreeVector(fBeamPos.x(), y, z));
  fGun->GeneratePrimaryVertex(evt); 
}