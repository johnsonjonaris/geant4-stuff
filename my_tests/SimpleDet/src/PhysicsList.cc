#include "PhysicsList.hh"

void MyPhysicsList::ConstructProcess()
{
    fMaxNumPhotonStep = 100;
	// Define transportation process
	AddTransportation();
	ConstructDecay();
	// electromagnetic processes
	ConstructEM();
	// Optical Physics
	ConstructOp();
}

void MyPhysicsList::ConstructEM()
{
	theParticleIterator->reset();
	while( (*theParticleIterator)() ){
		G4ParticleDefinition* particle = theParticleIterator->value();
		G4ProcessManager* pmanager = particle->GetProcessManager();
		G4String particleName = particle->GetParticleName();

		if (particleName == "gamma") {
			// gamma
			// Construct processes for gamma
			G4GammaConversion* fPairProduction = new G4GammaConversion();
			pmanager->AddDiscreteProcess(fPairProduction);
			G4ComptonScattering* theComptonScattering = new G4ComptonScattering();
			theComptonScattering->SetEmModel(new G4PenelopeComptonModel());
			pmanager->AddDiscreteProcess(theComptonScattering);
			G4PhotoElectricEffect* thePhotoElectricEffect = new G4PhotoElectricEffect();
			thePhotoElectricEffect->SetEmModel(new G4PenelopePhotoElectricModel());
			pmanager->AddDiscreteProcess(thePhotoElectricEffect);

		} else if (particleName == "e-") {
			//electron
			// Construct processes for electron
			pmanager->AddProcess(new G4eMultipleScattering(),-1, 1, 1);
			pmanager->AddProcess(new G4eIonisation(),       -1, 2, 2);
			pmanager->AddProcess(new G4eBremsstrahlung(),   -1, 3, 3);

		} else if (particleName == "e+") {
			//positron
			// Construct processes for positron
			pmanager->AddProcess(new G4eMultipleScattering(),-1, 1, 1);
			pmanager->AddProcess(new G4eIonisation(),       -1, 2, 2);
			pmanager->AddProcess(new G4eBremsstrahlung(),   -1, 3, 3);
			pmanager->AddProcess(new G4eplusAnnihilation(),  0,-1, 4);

		} else if ( particleName == "mu+" ||
					particleName == "mu-"    ) {
			//muon
			// Construct processes for muon
			pmanager->AddProcess(new G4MuMultipleScattering(),-1, 1, 1);
			pmanager->AddProcess(new G4MuIonisation(),      -1, 2, 2);
			pmanager->AddProcess(new G4MuBremsstrahlung(),  -1, 3, 3);
			pmanager->AddProcess(new G4MuPairProduction(),  -1, 4, 4);
			if(particleName == "mu-") {
				pmanager->AddRestProcess(new G4MuonMinusCapture());
			}

		} else {
			if ((particle->GetPDGCharge() != 0.0) &&
					(particle->GetParticleName() != "chargedgeantino") &&
					!particle->IsShortLived()) {
				// all others charged particles except geantino
				pmanager->AddProcess(new G4hMultipleScattering(),-1,1,1);
				pmanager->AddProcess(new G4hIonisation(),       -1,2,2);
			}
		}
	}
}

void MyPhysicsList::ConstructParticle()
{
	// In this method, static member functions should be called
	// for all particles which you want to use.
	// This ensures that objects of these particle types will be
	// created in the program.

	G4BosonConstructor bConstructor;
	bConstructor.ConstructParticle();
	/* Equivalent to:
	// gamma
	G4Gamma::GammaDefinition();

	// pseudo-particles
	// https://geant4.web.cern.ch/geant4/UserDocumentation/UsersGuides/ForApplicationDeveloper/html/ch05s03.html
	// virtual particles for simulation which do not interact with materials and undertake transportation processes only.
	G4Geantino::GeantinoDefinition();
	G4ChargedGeantino::ChargedGeantinoDefinition();

	// optical photon
	G4OpticalPhoton::OpticalPhotonDefinition();
	*/
	G4LeptonConstructor lConstructor;
	lConstructor.ConstructParticle();
	/* Equivalent to:
	// electron
	G4Electron::ElectronDefinition();
	G4Positron::PositronDefinition();
	G4NeutrinoE::NeutrinoEDefinition();
	G4AntiNeutrinoE::AntiNeutrinoEDefinition();

	// Mu
	G4MuonPlus::MuonPlusDefinition();
	G4MuonMinus::MuonMinusDefinition();
	G4NeutrinoMu::NeutrinoMuDefinition();
	G4AntiNeutrinoMu::AntiNeutrinoMuDefinition();
	*/

	G4MesonConstructor mConstructor;
	mConstructor.ConstructParticle();
	/* Equivalent to:
	//These are needed for the mu- capture
	G4PionMinus::PionMinus();
	G4PionZero::PionZero();
	G4PionPlus::PionPlus();
	 and much more
	*/

	G4BaryonConstructor rConstructor;
	rConstructor.ConstructParticle();
	/* Equivalent to:
	//These are needed for the mu- capture
	G4Neutron::Neutron();
	G4Proton::Proton();
	*/

	G4IonConstructor iConstructor;
	iConstructor.ConstructParticle();
	// Equivalent to:
	// G4GenericIon::GenericIonDefinition();
}

void MyPhysicsList::ConstructDecay()
{
	// Add Decay Process
	G4Decay* theDecayProcess = new G4Decay();
	theParticleIterator->reset();
	while( (*theParticleIterator)() ){
		G4ParticleDefinition* particle = theParticleIterator->value();
		G4ProcessManager* pmanager = particle->GetProcessManager();
		if (theDecayProcess->IsApplicable(*particle)) {
			pmanager ->AddProcess(theDecayProcess);
			// set ordering for PostStepDoIt and AtRestDoIt
			pmanager ->SetProcessOrdering(theDecayProcess, idxPostStep);
			pmanager ->SetProcessOrdering(theDecayProcess, idxAtRest);
		}
	}
}

void MyPhysicsList::ConstructOp()
{
	G4Cerenkov* cerenkovProcess = new G4Cerenkov("Cerenkov");
	cerenkovProcess->SetMaxNumPhotonsPerStep(fMaxNumPhotonStep);
	cerenkovProcess->SetMaxBetaChangePerStep(10.0);
	cerenkovProcess->SetTrackSecondariesFirst(true);
	G4Scintillation* scintillationProcess = new G4Scintillation("Scintillation");
	scintillationProcess->SetScintillationYieldFactor(1.);
	scintillationProcess->SetScintillationExcitationRatio(0.0);
	scintillationProcess->SetTrackSecondariesFirst(true);
	G4OpAbsorption* absorptionProcess = new G4OpAbsorption();
	G4RayleighScattering* rayleighScatteringProcess = new G4RayleighScattering();
	rayleighScatteringProcess->SetEmModel(new G4PenelopeRayleighModel());
	G4OpMieHG* mieHGScatteringProcess = new G4OpMieHG();
	G4OpBoundaryProcess* boundaryProcess = new G4OpBoundaryProcess();

	//	cerenkovProcess->SetVerboseLevel(fVerboseLebel);
	//	scintillationProcess->SetVerboseLevel(fVerboseLebel);
	//	absorptionProcess->SetVerboseLevel(fVerboseLebel);
	//	rayleighScatteringProcess->SetVerboseLevel(fVerboseLebel);
	//	mieHGScatteringProcess->SetVerboseLevel(fVerboseLebel);
	//	boundaryProcess->SetVerboseLevel(fVerboseLebel);

	// Use Birks Correction in the Scintillation process
	if(!G4Threading::IsWorkerThread())
	{
		G4EmSaturation* emSaturation =
				G4LossTableManager::Instance()->EmSaturation();
		scintillationProcess->AddSaturation(emSaturation);
	}

	theParticleIterator->reset();
	while( (*theParticleIterator)() ){
		G4ParticleDefinition* particle = theParticleIterator->value();
		G4ProcessManager* pmanager = particle->GetProcessManager();
		G4String particleName = particle->GetParticleName();
		if (cerenkovProcess->IsApplicable(*particle)) {
			pmanager->AddProcess(cerenkovProcess);
			pmanager->SetProcessOrdering(cerenkovProcess,idxPostStep);
		}
		if (scintillationProcess->IsApplicable(*particle)) {
			pmanager->AddProcess(scintillationProcess);
			pmanager->SetProcessOrderingToLast(scintillationProcess, idxAtRest);
			pmanager->SetProcessOrderingToLast(scintillationProcess, idxPostStep);
		}
		if (particleName == "opticalphoton") {
			G4cout << " AddDiscreteProcess to OpticalPhoton " << G4endl;
			pmanager->AddDiscreteProcess(absorptionProcess);
			pmanager->AddDiscreteProcess(rayleighScatteringProcess);
			pmanager->AddDiscreteProcess(mieHGScatteringProcess);
			pmanager->AddDiscreteProcess(boundaryProcess);
		}
	}
}

void MyPhysicsList::SetNbOfPhotonsCerenkov(G4int MaxNumber)
{
	fMaxNumPhotonStep = MaxNumber;
}

void MyPhysicsList::SetCuts()
{
	//  " G4VUserPhysicsList::SetCutsWithDefault" method sets
	//   the default cut value for all particle types
	//
	// according to paper, set cut value
	defaultCutValue = 0.5*um;
	SetCutsWithDefault();

	if (verboseLevel>0) DumpCutValuesTable();
}

