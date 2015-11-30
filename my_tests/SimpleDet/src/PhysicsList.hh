#ifndef PHYSICSLIST_HH
#define PHYSICSLIST_HH

#include "G4PhotoElectricEffect.hh"
#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4Scintillation.hh"
#include "G4PenelopeComptonModel.hh"
#include "G4RayleighScattering.hh"
#include "G4PenelopeRayleighModel.hh"
#include "G4PenelopePhotoElectricModel.hh"

#include "QBBC.hh"

class MyPhysicsList : G4VUserPhysicsList
{
	void ConstructEM();
public:
	void ConstructProcess();
};

#endif
