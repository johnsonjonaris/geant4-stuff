#include "G4VUserDetectorConstruction.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalVolume.hh"
#include "G4SystemOfUnits.hh"

class BasicDetectorConstruction : public G4VUserDetectorConstruction
{
public: 

    G4VPhysicalVolume* Construct()
    {
        //Obtain pointer to NIST material manager
        G4NistManager* nist = G4NistManager::Instance();

        //Build materials for world and target
        G4Material* CsI = G4Material::GetMaterial("G4_CESIUM_IODIDE");
        G4Material* air = nist->FindOrBuildMaterial("G4_AIR");

        //Define the world solid. The world will be 5x5x5 m3
        G4double env_sizeXY = 2.5*m;
        G4double env_sizeZ = 3.*m;
        G4double world_sizeXY = 1.2*env_sizeXY;
        G4double world_sizeZ  = 1.2*env_sizeZ;
        G4Box* solidWorld =
                new G4Box("World",world_sizeXY,world_sizeXY,world_sizeZ);

        //Fill the world with air
        G4LogicalVolume* logicWorld =
                new G4LogicalVolume(solidWorld, air, "World");

        //
        // Envelope
        //
        G4Box* solidEnv =
          new G4Box("Envelope",                    //its name
              0.5*env_sizeXY, 0.5*env_sizeXY, 0.5*env_sizeZ); //its size

        G4LogicalVolume* logicEnv =
          new G4LogicalVolume(solidEnv,            //its solid
                              air,             //its material
                              "Envelope");         //its name

        new G4PVPlacement(0,                       //no rotation
                          G4ThreeVector(),         //at (0,0,0)
                          logicEnv,                //its logical volume
                          "Envelope",              //its name
                          logicWorld,              //its mother  volume
                          false,                   //no boolean operation
                          0,                       //copy number
                          true);          //overlaps checking

        //Create the world physical volume. The world is the only
        //physical volume with no mother volume.
        G4VPhysicalVolume* physWorld =
                new G4PVPlacement(0,                       //no rotation
                                  G4ThreeVector(),       //at (0,0,0)
                                  logicWorld,            //its logical volume
                                  "World",               //its name
                                  0,                     //its mother  volume
                                  false,                 //no boolean operation
                                  0,                     //copy number
                                  true);			       //overlaps checking

        //Create  the shape of a target to fire particles at
        G4double target_sizeX=15.*cm;
        G4double target_sizeY=15.*cm;
        G4double target_sizeZ=40.*mm;
        G4Box* solidDetector =
                new G4Box("Target",target_sizeX, target_sizeY, target_sizeZ);

        //Create the target logical volume by
        //assigning the material of the target to be Pb
        G4LogicalVolume* logicDetector =
                new G4LogicalVolume(solidDetector, CsI, "myTarget");

        //Create the target physical volume by placing it in the
        //"logicWorld" logical volume.
        G4VPhysicalVolume* physDetector =
                new G4PVPlacement(0,                       //no rotation
                                  G4ThreeVector(0.,0.,70.*cm),
                                  logicDetector,           //its logical volume
                                  "World",               //its name
                                  logicWorld,             //its mother  volume
                                  false,                 //no boolean operation
                                  0,                     //copy number
                                  true);			       //overlaps checking
        return physWorld;
    }
};
