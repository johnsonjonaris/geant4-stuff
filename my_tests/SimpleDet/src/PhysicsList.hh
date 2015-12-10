#ifndef PHYSICSLIST_HH
#define PHYSICSLIST_HH

#include "globals.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"

#include "G4BosonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4ShortLivedConstructor.hh"


#include "G4ComptonScattering.hh"
#include "G4PenelopeComptonModel.hh"
#include "G4GammaConversion.hh"
#include "G4PhotoElectricEffect.hh"
#include "G4PenelopePhotoElectricModel.hh"

#include "G4eMultipleScattering.hh"
#include "G4MuMultipleScattering.hh"
#include "G4hMultipleScattering.hh"

#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"

#include "G4MuIonisation.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"
#include "G4MuonMinusCapture.hh"

#include "G4hIonisation.hh"

#include "G4ProcessManager.hh"

#include "G4Cerenkov.hh"
#include "G4Scintillation.hh"
#include "G4OpAbsorption.hh"

#include "G4PenelopeRayleighModel.hh"
#include "G4RayleighScattering.hh"
#include "G4OpRayleigh.hh"

#include "G4OpMieHG.hh"
#include "G4OpBoundaryProcess.hh"

#include "G4LossTableManager.hh"
#include "G4EmSaturation.hh"

#include "G4Threading.hh"
#include "G4SystemOfUnits.hh"

#include "G4Decay.hh"

#include "QBBC.hh"

class MyPhysicsList : G4VUserPhysicsList
{
    G4int fMaxNumPhotonStep;
public:
	//these methods Construct physics processes and register them
	void ConstructEM();
	// Optical Physics
	void ConstructOp();
	void ConstructDecay();

	virtual void SetCuts();

	virtual void ConstructProcess();
	virtual void ConstructParticle();

	void SetNbOfPhotonsCerenkov(G4int);
};

#endif
