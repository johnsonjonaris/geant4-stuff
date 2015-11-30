#include "PhysicsList.hh"

void MyPhysicsList::ConstructProcess()
{
	// Define transportation process
	AddTransportation();
	// electromagnetic processes
	ConstructEM();
}

void MyPhysicsList::ConstructEM()
{
	// Get pointer to G4PhysicsListHelper
	G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();

	//  Get pointer to gamma
	G4ParticleDefinition* particle = G4Gamma::GammaDefinition();

	// Construct and register processes for gamma
	// control scintillation factor to 400 Photon/MeV
	/*
	G4double beta = 400;
	G4Scintillation *scin = new G4Scintillation();
	scin->SetScintillationYieldFactor(beta);
	ph->RegisterProcess(scin, particle);
	*/
	G4PhotoElectricEffect* thePhotoElectricEffect = new G4PhotoElectricEffect();
	thePhotoElectricEffect-> SetEmModel(new G4PenelopePhotoElectricModel());
	ph->RegisterProcess(thePhotoElectricEffect, particle);
	G4ComptonScattering* theComptonScattering = new G4ComptonScattering();
	theComptonScattering-> SetEmModel(new G4PenelopeComptonModel());
	ph->RegisterProcess(theComptonScattering, particle);
	ph->RegisterProcess(new G4GammaConversion(), particle);
}
