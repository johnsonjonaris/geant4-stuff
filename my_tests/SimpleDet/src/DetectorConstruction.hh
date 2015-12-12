#ifndef DETECTORCONSTRUCTION_HH
#define DETECTORCONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalVolume.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Cache.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalSkinSurface.hh"

#include "ScintSD.hh"
#include "PhotoCathodeSD.hh"

class BasicDetectorConstruction : public G4VUserDetectorConstruction
{
    G4double scintElementWidth, scintElementHeight;
    G4double scintElementThickness;
    G4double scintElementWallThickness;
    G4double scintElementCoverThickness;
    G4double photoCathodeThickness;

    G4int nx, ny;

    G4Material *air;
    G4Element *fH;
    G4Element *fC;
    G4Material *Pstyrene;
    G4Material *CsI;
    G4Material *Cu;
    G4Material *Al;

    G4Box *solidWorld;
    G4Box *scintElement_box;
    G4Box *scintElementCover_box;
    G4Box *scintElementWalls_box;
    G4Box *photoCathode_box;

    G4VPhysicalVolume* physWorld;
    G4LogicalVolume* logicWorld;

    G4LogicalVolume *scintElement_log;
    G4LogicalVolume *scintElementCover_log;
    G4LogicalVolume *scintElementWalls_log;
    G4LogicalVolume *photoCathode_log;

    // sensitive detector
    G4Cache<ScintSD*> scintSD;
    G4Cache<PhotoCathodeSD*> photoCathodeSD;

    // Sensitive Detectors positions
    std::vector<G4ThreeVector> scintElementPositions;

    void initDefaultValues();

    void defineMaterials();
    void defineScintillatorElement();
    void setVisualAttributes();
    void setOpticalProperties();


    void setDefaultValues();
    void placeScintElements();
public: 

    BasicDetectorConstruction();

	G4VPhysicalVolume* Construct();
    virtual void ConstructSDandField();

};

#endif
