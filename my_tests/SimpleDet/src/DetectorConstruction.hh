#ifndef DETECTORCONSTRUCTION_HH
#define DETECTORCONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalVolume.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"

class BasicDetectorConstruction : public G4VUserDetectorConstruction
{
    G4double scintElementWidth, scintElementHeight;
    G4double scintElementThickness;
    G4double scintElementWallThickness;
    G4double scintElementCoverThickness;

    G4int nx, ny;

    G4Box *scintElement_box;
    G4Box *scintElementCover_box;
    G4Box *scintElementWalls_box;

    G4VPhysicalVolume* physWorld;
    G4LogicalVolume* logicWorld;

    G4LogicalVolume *scintElement_log;
    G4LogicalVolume *scintElementCover_log;
    G4LogicalVolume *scintElementWalls_log;

    // Sensitive Detectors positions
    std::vector<G4ThreeVector> scintElementPositions;

    void initDefaultValues();

    void defineScintillatorElement();
    void setVisualAttributes();
    void setOpticalProperties();


    void setDefaultValues();
    void placeScintElements();
public: 

	G4VPhysicalVolume* Construct();
};

#endif
