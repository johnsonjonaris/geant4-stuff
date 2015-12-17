#include "DetectorConstruction.hh"
#include "BasicPrimaryGeneratorAction.hh"
#include "PhysicsList.hh"
#include "EventAction.hh"
//#include "SteppingAction.hh"
#include "TrackingAction.hh"
#include "StackingAction.hh"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"


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
    runManager->SetUserInitialization(new MyPhysicsList());

	// Primary generator action
	runManager->SetUserAction(new BasicPrimaryGeneratorAction());
    // set other actions
    runManager->SetUserAction(new StackingAction());
    runManager->SetUserAction(new EventAction());
    runManager->SetUserAction(new TrackingAction());
    runManager->SetUserAction(new SteppingAction());

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
