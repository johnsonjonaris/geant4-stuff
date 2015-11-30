#include "DetectorConstruction.hh"

G4VPhysicalVolume* BasicDetectorConstruction::Construct()
{
	//Obtain pointer to NIST material manager
	G4NistManager* nist = G4NistManager::Instance();

	//Build materials for world and target
	G4Material* CsI = nist->FindOrBuildMaterial("G4_CESIUM_FLUORIDE");
	G4Material* air = nist->FindOrBuildMaterial("G4_AIR");

	//Define the world solid
	G4double world_sizeXY = 1.2*m;
	G4double world_sizeZ  = 2.*m;

	G4Box* solidWorld =
			new G4Box("World",world_sizeXY,world_sizeXY,world_sizeZ);
	//Fill the world with air
	G4LogicalVolume* logicWorld =
			new G4LogicalVolume(solidWorld, air, "World");
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
	// 15 X 15 X 40
	G4double target_sizeX=7.5*cm;
	G4double target_sizeY=7.5*cm;
	G4double target_sizeZ=20.*mm;
	G4Box* solidDetector =
			new G4Box("Detector",target_sizeX, target_sizeY, target_sizeZ);

	//Create the target logical volume by
	//assigning the material of the target to be Pb
	G4LogicalVolume* logicDetector =
			new G4LogicalVolume(solidDetector, CsI, "Detector");

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

