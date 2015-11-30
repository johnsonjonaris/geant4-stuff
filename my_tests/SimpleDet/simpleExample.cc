#include "DetectorConstruction.cc"
#include "BasicPrimaryGeneratorAction.cc"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "QBBC.hh"

#include "G4PhotoElectricEffect.hh"
#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4Scintillation.hh"
#include "G4PenelopeComptonModel.hh"
#include "G4RayleighScattering.hh"
#include "G4PenelopeRayleighModel.hh"
#include "G4PenelopePhotoElectricModel.hh"

#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

class MyPhysicsList : G4VUserPhysicsList
{
	void ConstructEM();
public:
	void ConstructProcess();
};

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

int main(int argc,char** argv){

	// Detect interactive mode (if no arguments) and define UI session
	//
	G4UIExecutive* ui = 0;
	if ( argc == 1 ) {
		ui = new G4UIExecutive(argc, argv);
	}

	G4RunManager * runManager = new G4RunManager;
	// Detector construction
	runManager->SetUserInitialization(new BasicDetectorConstruction());

	// Physics list
	// the physics list must always be registered before the primary generator action can
	// be instantiated since otherwise the relevant particle definitions may not be available.
	G4VModularPhysicsList* physicsList = new QBBC;
	runManager->SetUserInitialization(physicsList);

	// Primary generator action
	runManager->SetUserAction(new BasicPrimaryGeneratorAction());

	// Initialize G4 kernel
	runManager->Initialize();

	// Initialize visualization
	//
	G4VisManager* visManager = new G4VisExecutive;
	// G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
	// G4VisManager* visManager = new G4VisExecutive("Quiet");
	visManager->Initialize();

	// Get the pointer to the User Interface manager
	G4UImanager* UImanager = G4UImanager::GetUIpointer();

	//Cause the run manager to generate a single event using the
	//primary generator action registered above.
	// runManager->BeamOn(1);

	// Process macro or start UI session
	//
	if ( ! ui ) {
		// batch mode
		G4String command = "/control/execute ";
		G4String fileName = argv[1];
		UImanager->ApplyCommand(command+fileName);
	}
	else {
		// interactive mode
		UImanager->ApplyCommand("/control/execute init_vis.mac");
		ui->SessionStart();
		delete ui;
	}

	//After the run is complete, free up the memory used by run
	//manager and return
	delete visManager;
	delete runManager;
	return 0;
}
