#include "DetectorConstruction.hh"

G4VPhysicalVolume* BasicDetectorConstruction::Construct()
{
    setDefaultValues();
    //Obtain pointer to NIST material manager
    G4NistManager* nist = G4NistManager::Instance();

    //Build materials for world and target
    G4Material* air = nist->FindOrBuildMaterial("G4_AIR");

    // ****************** Define the world solid ************************
    G4double world_sizeXY = 1.2*m;
    G4double world_sizeZ  = 2.*m;

    G4Box* solidWorld =
            new G4Box("World",world_sizeXY,world_sizeXY,world_sizeZ);
    //Fill the world with air
    logicWorld = new G4LogicalVolume(solidWorld, air, "World");
    //Create the world physical volume. The world is the only
    //physical volume with no mother volume.
    physWorld =
            new G4PVPlacement(0,                       //no rotation
                              G4ThreeVector(),       //at (0,0,0)
                              logicWorld,            //its logical volume
                              "World",               //its name
                              0,                     //its mother  volume
                              false,                 //no boolean operation
                              0,                     //copy number
                              true);			       //overlaps checking

    // ****************** Define the scintillator ************************
    // create scintillator element
    defineScintillatorElement();
    // place the scintillator elements
    placeScintElements();

    return physWorld;
}

void BasicDetectorConstruction::placeScintElements()
{
    G4double dx = scintElementWidth+scintElementWallThickness;
    G4double dy = scintElementHeight+scintElementWallThickness;
    G4double x = -nx/2*dx + dx/2.0;
    G4double y = -ny/2*dy + dy/2.0;
    G4double z = -70.*cm;
    G4int k = 0;

    for (int j = 0; j < ny; ++j) {
        x = -nx/2*dx + dx/2.0;
        for(int i = 0; i < nx; ++i) {
            //printf("%f, %f, %f\n", x, y, z);
            new G4PVPlacement(0, G4ThreeVector(x,y,z), scintElementWalls_log, "scint_el",
                              logicWorld, false, k);
            scintElementPositions.push_back(G4ThreeVector(x,y,z));
            x += dx;
            k++;
        }
        y += dy;
    }
}

void BasicDetectorConstruction::defineScintillatorElement()
{
    //Materials
    //Polystyrene
    G4NistManager* nist = G4NistManager::Instance();
    G4Element *fH = new G4Element("H", "H", 1., 1.01*g/mole);
    G4Element *fC = new G4Element("C", "C", 6., 12.01*g/mole);

    G4Material *Pstyrene = new G4Material("Polystyrene", 1.03*g/cm3, 2);
    Pstyrene->AddElement(fC, 8);
    Pstyrene->AddElement(fH, 8);

    G4double wls_Energy[] = {2.00*eV,2.87*eV,2.90*eV,3.47*eV};
    const G4int wlsnum = sizeof(wls_Energy)/sizeof(G4double);
    G4double rIndexPstyrene[]={ 1.5, 1.5, 1.5, 1.5};
    G4double absorption1[]={2.*cm, 2.*cm, 2.*cm, 2.*cm};
    G4double scintilFast[]={0.00, 0.00, 1.00, 1.00};
    G4MaterialPropertiesTable *MPTPStyrene = new G4MaterialPropertiesTable();
    MPTPStyrene->AddProperty("RINDEX",wls_Energy,rIndexPstyrene,wlsnum);
    MPTPStyrene->AddProperty("ABSLENGTH",wls_Energy,absorption1,wlsnum);
    MPTPStyrene->AddProperty("FASTCOMPONENT",wls_Energy, scintilFast,wlsnum);
    MPTPStyrene->AddConstProperty("SCINTILLATIONYIELD",10./keV);
    MPTPStyrene->AddConstProperty("RESOLUTIONSCALE",1.0);
    MPTPStyrene->AddConstProperty("FASTTIMECONSTANT", 10.*ns);
    Pstyrene->SetMaterialPropertiesTable(MPTPStyrene);

    G4Material* CsI = nist->FindOrBuildMaterial("G4_CESIUM_FLUORIDE");
    G4Material* CU = nist->FindOrBuildMaterial("G4_Cu");
    /*
        define scintillator element
        the element is a box made of CsI and of 40 mm thickness
        its walls (0.05 um thickness) are covered by Polystyrene
        the top is covered by a copper layer of 1 mm thickness
        the walls extends to cover the top copper cover

          1mm  40 mm
          ---------------
          ---------------
          | |           |
          | |           |
          ---------------
          ---------------
      */
    scintElement_box = new G4Box("scint_box",
                                 scintElementWidth/2.,
                                 scintElementHeight/2.,
                                 scintElementThickness/2.);
    G4double wallHeight = scintElementCoverThickness + scintElementThickness;
    scintElementWalls_box = new G4Box("walls_box",
                                      (scintElementWidth+scintElementWallThickness)/2.,
                                      (scintElementHeight+scintElementWallThickness)/2.,
                                      wallHeight/2.);
    scintElementCover_box = new G4Box("cover_box",
                                      scintElementWidth/2.,
                                      scintElementHeight/2.,
                                      scintElementCoverThickness/2.);

    scintElement_log = new G4LogicalVolume(scintElement_box, CsI, "scint_log",0,0,0);
    scintElementWalls_log = new G4LogicalVolume(scintElementWalls_box, Pstyrene, "walls_log",0,0,0);
    scintElementCover_log = new G4LogicalVolume(scintElementCover_box, CU, "cover_log",0,0,0);

    new G4PVPlacement(0,                                    // no rotation
                      G4ThreeVector(0.,0.,
                                    -scintElementCoverThickness/2.0),   // translation
                      scintElement_log,                            // the logical volume
                      "scint",                              // name
                      scintElementWalls_log,                            // mother volume
                      false,                                // no boolean operation
                      0);                                   // its copy number

    new G4PVPlacement(0,                                    // no rotation
                      G4ThreeVector(0.,0.,                  // translation
                                    wallHeight/2.0 - scintElementCoverThickness/2.0),
                      scintElementCover_log,                            // the logical volume
                      "cu_cover",                           // name
                      scintElementWalls_log,                            // mother volume
                      false,                                // no boolean operation
                      0);                                   // its copy number

    setVisualAttributes();
    setOpticalProperties();
}

void BasicDetectorConstruction::setVisualAttributes()
{
    // set color attributes
    G4VisAttributes* scint_va = new G4VisAttributes(G4Colour(0.8,0.8,0.8));
    scintElement_log->SetVisAttributes(scint_va);

    G4VisAttributes* walls_va = new G4VisAttributes(G4Colour(1.0,1.0,0.0));
    walls_va->SetForceSolid(true);
    scintElementWalls_log->SetVisAttributes(walls_va);

    G4VisAttributes* cover_va = new G4VisAttributes(G4Colour(1.0,0.0,0.0));
    scintElementCover_log->SetVisAttributes(cover_va);
}

void BasicDetectorConstruction::setOpticalProperties()
{

}

void BasicDetectorConstruction::setDefaultValues()
{
    // values according to: Wang et. al. "High-DQE EPIDs based on thick, segmented BGO
    // and CsI:Tl scintillators: Performance evaluation at extremely low dose"
    scintElementWidth = 1.016*mm;
    scintElementHeight = 1.016*mm;
    scintElementThickness = 40.0*mm;
    scintElementWallThickness = 0.05*um;
    scintElementCoverThickness = 1.0*mm;
    // scintitllator is made of a 150X150 grid
    nx = 150;
    ny = 150;
}
