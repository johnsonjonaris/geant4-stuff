#include "PhysicsList.hh"

void MuonPhysics::ConstructParticle()
{
    // Mu
    G4MuonPlus::MuonPlusDefinition();
    G4MuonMinus::MuonMinusDefinition();
    G4NeutrinoMu::NeutrinoMuDefinition();
    G4AntiNeutrinoMu::AntiNeutrinoMuDefinition();
    //These are needed for the mu- capture
    G4Neutron::Neutron();
    G4Proton::Proton();
    G4PionMinus::PionMinus();
    G4PionZero::PionZero();
    G4PionPlus::PionPlus();
}

void MuonPhysics::ConstructProcess()
{
    G4ProcessManager * pManager = 0;

    // Muon Plus Physics
    pManager = G4MuonPlus::MuonPlus()->GetProcessManager();
    pManager->AddProcess(new G4MuIonisation(),      -1,  1, 1);
    pManager->AddProcess(new G4MuIonisation(),      -1,  2, 2);
    pManager->AddProcess(new G4MuBremsstrahlung(),  -1,  3, 3);
    pManager->AddProcess(new G4MuPairProduction(),  -1,  4, 4);

    // Muon Minus Physics
    pManager = G4MuonMinus::MuonMinus()->GetProcessManager();
    pManager->AddProcess(new G4MuIonisation(),      -1,  1, 1);
    pManager->AddProcess(new G4MuIonisation(),      -1,  2, 2);
    pManager->AddProcess(new G4MuBremsstrahlung(),  -1,  3, 3);
    pManager->AddProcess(new G4MuPairProduction(),  -1,  4, 4);
    pManager->AddRestProcess(new G4MuonMinusCapture());
}

void GeneralPhysics::ConstructParticle()
{
    // pseudo-particles
    // https://geant4.web.cern.ch/geant4/UserDocumentation/UsersGuides/ForApplicationDeveloper/html/ch05s03.html
    // virtual particles for simulation which do not interact with materials and undertake transportation processes only.
    G4Geantino::GeantinoDefinition();
    G4ChargedGeantino::ChargedGeantinoDefinition();
    G4GenericIon::GenericIonDefinition();
}

void GeneralPhysics::ConstructProcess()
{
    G4Decay* fDecayProcess = new G4Decay();
    // Add Decay Process
    aParticleIterator->reset();
    while( (*aParticleIterator)() ){
        G4ParticleDefinition* particle = aParticleIterator->value();
        G4ProcessManager* pmanager = particle->GetProcessManager();
        if (fDecayProcess->IsApplicable(*particle)) {
            pmanager ->AddProcess(fDecayProcess);
            // set ordering for PostStepDoIt and AtRestDoIt
            pmanager ->SetProcessOrdering(fDecayProcess, idxPostStep);
            pmanager ->SetProcessOrdering(fDecayProcess, idxAtRest);
        }
    }
}

void EMPhysics::ConstructParticle()
{
    // gamma
    G4Gamma::GammaDefinition();

    // electron
    G4Electron::ElectronDefinition();
    G4Positron::PositronDefinition();
    G4NeutrinoE::NeutrinoEDefinition();
    G4AntiNeutrinoE::AntiNeutrinoEDefinition();
}

void EMPhysics::ConstructProcess()
{
    G4ProcessManager* pManager = 0;

    // Gamma Physics
    pManager = G4Gamma::Gamma()->GetProcessManager();
    pManager = G4Gamma::Gamma()->GetProcessManager();
    G4PhotoElectricEffect *photoElectricEffect = new G4PhotoElectricEffect();
    photoElectricEffect->SetEmModel(new G4PenelopePhotoElectricModel());
    pManager->AddDiscreteProcess(photoElectricEffect);
    G4ComptonScattering *comptonScattering = new G4ComptonScattering();
    comptonScattering->SetEmModel(new G4PenelopeComptonModel());
    pManager->AddDiscreteProcess(comptonScattering);
    G4GammaConversion *gammaConversion = new G4GammaConversion();
    gammaConversion->SetEmModel(new G4PenelopeGammaConversionModel());
    pManager->AddDiscreteProcess(gammaConversion);

    // Electron Physics
    pManager = G4Electron::Electron()->GetProcessManager();
    G4eIonisation *eIonisation1 = new G4eIonisation();
    eIonisation1->SetEmModel(new G4PenelopeIonisationModel());
    G4eBremsstrahlung *eBremsstrahlung1 = new G4eBremsstrahlung();
    eBremsstrahlung1->SetEmModel(new G4PenelopeIonisationModel());
    pManager->AddProcess(new G4eMultipleScattering(),   -1, 1, 1);
    pManager->AddProcess(eIonisation1,                  -1, 2, 2);
    pManager->AddProcess(eBremsstrahlung1,              -1, 3, 3);

    //Positron Physics
    pManager = G4Positron::Positron()->GetProcessManager();
    G4eIonisation *eIonisation2 = new G4eIonisation();
    eIonisation2->SetEmModel(new G4PenelopeIonisationModel());
    G4eBremsstrahlung *eBremsstrahlung2 = new G4eBremsstrahlung();
    eBremsstrahlung2->SetEmModel(new G4PenelopeIonisationModel());
    G4eplusAnnihilation *eplusAnnihilation = new G4eplusAnnihilation();
    eplusAnnihilation->SetEmModel(new G4PenelopeAnnihilationModel);
    pManager->AddProcess(new G4eMultipleScattering(),   -1, 1, 1);
    pManager->AddProcess(eIonisation2,                  -1, 2, 2);
    pManager->AddProcess(eBremsstrahlung2,              -1, 3, 3);
    pManager->AddProcess(eplusAnnihilation,             0,-1, 4);

}

MyPhysicsList::MyPhysicsList() : G4VModularPhysicsList()
{
    // General Physics
    RegisterPhysics( new GeneralPhysics("general") );
    // EM Physics
    RegisterPhysics( new EMPhysics("standard EM"));
    // Muon Physics
    RegisterPhysics( new MuonPhysics("muon"));

    // Optical Physics
    G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();
    RegisterPhysics( opticalPhysics );

    opticalPhysics->SetWLSTimeProfile("delta");

    opticalPhysics->SetScintillationYieldFactor(1.0);
    opticalPhysics->SetScintillationExcitationRatio(0.0);

    opticalPhysics->SetMaxNumPhotonsPerStep(100);
    opticalPhysics->SetMaxBetaChangePerStep(10.0);

    opticalPhysics->SetTrackSecondariesFirst(kCerenkov,true);
    opticalPhysics->SetTrackSecondariesFirst(kScintillation,true);
}

void MyPhysicsList::SetCuts()
{
    //  " G4VUserPhysicsList::SetCutsWithDefault" method sets
    //   the default cut value for all particle types
    //
    // according to paper, set cut value
    defaultCutValue = 0.5*um;
    printf("Range cut = %f\n", defaultCutValue);

    SetCutsWithDefault();

    if (verboseLevel>0) DumpCutValuesTable();
}

