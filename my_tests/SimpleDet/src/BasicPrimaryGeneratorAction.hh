#ifndef BASICPRIMARYGERNERATORACTION_H
#define BASICPRIMARYGERNERATORACTION_H

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4Gamma.hh"
#include "G4SystemOfUnits.hh"
#include "G4RandomDirection.hh"
#include "G4GeneralParticleSource.hh"
#include "globals.hh"
#include "G4ParticleTable.hh"

class G4ParticleGun;

class BasicPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{

public:
	BasicPrimaryGeneratorAction();
	virtual ~BasicPrimaryGeneratorAction();

	void GeneratePrimaries(G4Event* anEvent);
private:
	G4ParticleGun*  fParticleGun; // pointer to a G4 gun class
};

#endif
