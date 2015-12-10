#include "BasicPrimaryGeneratorAction.hh"

BasicPrimaryGeneratorAction::BasicPrimaryGeneratorAction()
	: G4VUserPrimaryGeneratorAction(),
	  fParticleGun(0)
{
	G4int n_particle = 1;
	fParticleGun  = new G4ParticleGun(n_particle);

	// default particle kinematic
	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4String particleName;
	G4ParticleDefinition* particle
			= particleTable->FindParticle(particleName="gamma");
	fParticleGun->SetParticleDefinition(particle);
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,-1.));
	fParticleGun->SetParticleEnergy(6.*MeV);
}

BasicPrimaryGeneratorAction::~BasicPrimaryGeneratorAction()
{
  delete fParticleGun;
}


void BasicPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{

	/*
	//Specify particle to be emitted
	// gun is focused on detector evenly flooding it
	// used for flooded image method
	G4double size = 0.8;
	G4double x0 = size * 15. * (G4UniformRand()-0.5);
	G4double y0 = size * 15. * (G4UniformRand()-0.5);
	fParticleGun->SetParticlePosition(G4ThreeVector(x0*cm, y0*cm, -70.*cm));
	*/
	// for the LSF method depending on PSF
    fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., 70.*cm));

	//Generate one instance of specified particle
	fParticleGun->GeneratePrimaryVertex(anEvent);
}
