#ifndef BASICPRIMARYGERNERATORACTION_H
#define BASICPRIMARYGERNERATORACTION_H

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4Proton.hh"
#include "G4Gamma.hh"
#include "G4SystemOfUnits.hh"
#include "G4RandomDirection.hh"
#include "G4GeneralParticleSource.hh"

class BasicPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction{
public:
	void GeneratePrimaries(G4Event* anEvent);
};

#endif
