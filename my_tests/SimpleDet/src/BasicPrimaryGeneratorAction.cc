#include "BasicPrimaryGeneratorAction.hh"

void BasicPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
	//Create particle gun
	G4ParticleGun* myGun = new G4ParticleGun();

	//Specify particle to be emitted
	myGun->SetParticleDefinition(G4Gamma::GammaDefinition());
	myGun->SetParticlePosition(G4ThreeVector(0.,0.,-70.*cm));
	//myGun->SetNumberOfParticles();
	//Set particle  kinetic energy and direction of travel
	myGun->SetParticleEnergy(6.*MeV);
	// travelling in the x axis
	myGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1));

	//myGun->SetParticleTime();

	// for a point source
	myGun->SetParticleMomentum(G4RandomDirection());

	//myGun->SetNumberOfParticles();
	//Generate one instance of specified particle
	myGun->GeneratePrimaryVertex(anEvent);
}
