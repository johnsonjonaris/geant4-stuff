#include "DetectorConstruction.hh"

BasicDetectorConstruction::BasicDetectorConstruction()
{
    setDefaultValues();
}

G4VPhysicalVolume* BasicDetectorConstruction::Construct()
{
    defineMaterials();

    // ****************** Define the world solid ************************
    G4double world_sizeXY = 1.2*m;
    G4double world_sizeZ  = 1.5*m;

    solidWorld = new G4Box("World",world_sizeXY,world_sizeXY,world_sizeZ);
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
    setVisualAttributes();
    setOpticalProperties();

    return physWorld;
}


void BasicDetectorConstruction::ConstructSDandField()
{
    if (!physWorld) return;
    // sensitive detector
    if (!photoCathodeSD.Get()) {
        G4cout << "Construction photocathode sensitive detector\n";
        photoCathodeSD.Put(new PhotoCathodeSD("PhotoCathodeSD"));
        photoCathodeSD.Get()->initScintillatorElements(nx*ny);
        photoCathodeSD.Get()->setPositions(scintElementPositions);
    }
    //sensitive detector need to be attached to something or else it doesnt get
    //reset at the begining of events
    SetSensitiveDetector(photoCathode_log, photoCathodeSD.Get());

    // Scint SD
    if (!scintSD.Get()) {
        G4cout << "Construction scintillator sensitive detector\n";
        scintSD.Put(new ScintSD("ScintillatorSD"));
    }
    SetSensitiveDetector(scintElement_log, scintSD.Get());
}

void BasicDetectorConstruction::defineMaterials()
{
    G4NistManager* nist = G4NistManager::Instance();
    air = nist->FindOrBuildMaterial("G4_AIR");
    G4double air_Energy[]={2.0*eV,7.0*eV,7.14*eV};
    const G4int airnum = sizeof(air_Energy)/sizeof(G4double);
    G4double air_RIND[]={1.,1.,1.};
    assert(sizeof(air_RIND) == sizeof(air_Energy));
    G4MaterialPropertiesTable *air_MPT = new G4MaterialPropertiesTable();
    air_MPT->AddProperty("RINDEX", air_Energy, air_RIND,airnum);
    air->SetMaterialPropertiesTable(air_MPT);

    //Polystyrene
    fH = new G4Element("H", "H", 1., 1.01*g/mole);
    fC = new G4Element("C", "C", 6., 12.01*g/mole);

    Pstyrene = new G4Material("Polystyrene", 1.03*g/cm3, 2);
    Pstyrene->AddElement(fC, 8);
    Pstyrene->AddElement(fH, 8);

    // does that need modification
    G4double PStyrene_Energy[] = {2.00*eV,2.87*eV,2.90*eV,3.47*eV};
    const G4int PStyrene_num = sizeof(PStyrene_Energy)/sizeof(G4double);
    G4double rIndexPstyrene[] = {1.5, 1.5, 1.5, 1.5};
    G4double absorption1[] = {2.*cm, 2.*cm, 2.*cm, 2.*cm};
    G4double scintilFast[] = {0.00, 0.00, 1.00, 1.00};
    G4MaterialPropertiesTable *PStyrene_MTP = new G4MaterialPropertiesTable();
    PStyrene_MTP->AddProperty("RINDEX", PStyrene_Energy,rIndexPstyrene,PStyrene_num);
    // absorption length
    PStyrene_MTP->AddProperty("ABSLENGTH", PStyrene_Energy,absorption1,PStyrene_num);
    PStyrene_MTP->AddProperty("FASTCOMPONENT", PStyrene_Energy, scintilFast,PStyrene_num);
    PStyrene_MTP->AddConstProperty("SCINTILLATIONYIELD", 10./keV);
    PStyrene_MTP->AddConstProperty("RESOLUTIONSCALE", 1.0);
    PStyrene_MTP->AddConstProperty("FASTTIMECONSTANT", 10.*ns);
    Pstyrene->SetMaterialPropertiesTable(PStyrene_MTP);
    Pstyrene->GetIonisation()->SetBirksConstant(0.126*mm/MeV);

    CsI = nist->FindOrBuildMaterial("G4_CESIUM_IODIDE");
    // 600 - 550 - 500 nm
    // A Gaussian-distributed number of photons is generated according to the energy lost
    // during the step.
    G4double CsI_Energy[] = { 2.07*eV , 2.25*eV, 2.48*eV };
    const G4int nCsIE = sizeof(CsI_Energy)/sizeof(G4double);
    G4double CsI_SCINT[] = { 0.1, 1.0, 0.1 };
    G4double CsI_RIND[]  = { 1.78 , 1.79, 1.8 };
    G4double CsI_ABSL[]  = { 33.*cm, 33.*cm, 33.*cm};
    G4MaterialPropertiesTable *CsI_MPT = new G4MaterialPropertiesTable();
    CsI_MPT->AddProperty("FASTCOMPONENT", CsI_Energy, CsI_SCINT, nCsIE);
    CsI_MPT->AddProperty("SLOWCOMPONENT", CsI_Energy, CsI_SCINT, nCsIE);
    CsI_MPT->AddProperty("RINDEX",        CsI_Energy, CsI_RIND,  nCsIE);
    CsI_MPT->AddProperty("ABSLENGTH",     CsI_Energy, CsI_ABSL,  nCsIE);
    /*
    Every scintillating material has a
        Characteristic light yield: SCINTILLATIONYIELD,
        Intrinsic resolution (RESOLUTIONSCALE): which generally broadens the statistical
                                                distribution of generated photons.
    A wider intrinsic resolution is due to impurities which are typical for doped crystals
    like NaI(Tl) and CsI(Tl).
    The intrinsic resolution can also be narrower when the Fano factor plays a role.

    A scintillator is also characterized by its photon emission spectrum and by the
    exponential decay of its time spectrum. The scintillator can have a fast (FASTTIMECONSTANT)
    and a slow (SLOWTIMECONSTANT) component. The relative strength of the fast component
    as a fraction of total scintillation yield is given by the (YIELDRATIO).
      */
    CsI_MPT->AddConstProperty("SCINTILLATIONYIELD",60000./MeV);
    CsI_MPT->AddConstProperty("RESOLUTIONSCALE",1.0);
    //CsI_mt->AddConstProperty("FASTTIMECONSTANT",20.*ns);
    CsI_MPT->AddConstProperty("SLOWTIMECONSTANT",680.*ns);
    CsI_MPT->AddConstProperty("YIELDRATIO",1.0);
    CsI->SetMaterialPropertiesTable(CsI_MPT);

    Cu = nist->FindOrBuildMaterial("G4_Cu");
    Al = nist->FindOrBuildMaterial("G4_Al");
}

void BasicDetectorConstruction::defineScintillatorElement()
{
    /*
        define scintillator element
        the element is a box made of CsI and of 40 mm thickness
        its walls (0.05 um thickness) are covered by Polystyrene
        the top is covered by a copper layer of 1 mm thickness
        the back is covered by an aluminum layer of 10 mm thickness
        that will absorb the photons
        the walls extends to cover the top and bottom covers

          1mm       40 mm   20 mm
          ------------------|---|
          ------------------|   |
          | |      _|_      |   |
          | |       |       |   |
          ------------------|   |
          ------------------|---|
      */
    scintElement_box = new G4Box("scint_box",
                                 scintElementWidth/2.,
                                 scintElementHeight/2.,
                                 scintElementThickness/2.);
    G4double wallHeight = scintElementCoverThickness +
                            scintElementThickness +
                            photoCathodeThickness;
    scintElementWalls_box = new G4Box("walls_box",
                                      (scintElementWidth+scintElementWallThickness)/2.,
                                      (scintElementHeight+scintElementWallThickness)/2.,
                                      wallHeight/2.);
    scintElementCover_box = new G4Box("cover_box",
                                      scintElementWidth/2.,
                                      scintElementHeight/2.,
                                      scintElementCoverThickness/2.);
    photoCathode_box = new G4Box("photocath_box",
                                 (scintElementWidth+scintElementWallThickness)/2.,
                                 (scintElementHeight+scintElementWallThickness)/2.,
                                  photoCathodeThickness/2.);

    scintElement_log = new G4LogicalVolume(scintElement_box, CsI, "scint_log",0,0,0);
    scintElementWalls_log = new G4LogicalVolume(scintElementWalls_box, Pstyrene, "walls_log",0,0,0);
    scintElementCover_log = new G4LogicalVolume(scintElementCover_box, Cu, "cover_log",0,0,0);
    //the "photocathode" is a metal slab at the back of the glass that
    //is only a very rough approximation of the real thing since it only
    //absorbs or detects the photons based on the efficiency set below
    photoCathode_log = new G4LogicalVolume(photoCathode_box, Al, "photocath_log",0,0,0);

    new G4PVPlacement(0,                                    // no rotation
                      G4ThreeVector(0.,0.,
                                    wallHeight/2. -
                                    scintElementThickness/2. -
                                    scintElementCoverThickness),   // translation
                      scintElement_log,                            // the logical volume
                      "scint",                              // name
                      scintElementWalls_log,                            // mother volume
                      false,                                // no boolean operation
                      0);                                   // its copy number

    new G4PVPlacement(0,
                      G4ThreeVector(0.,0.,
                                    wallHeight/2. -
                                    scintElementCoverThickness/2.),
                      scintElementCover_log,
                      "cu_cover",
                      scintElementWalls_log,
                      false,
                      0);
    new G4PVPlacement(0,
                      G4ThreeVector(0.,0.,
                                    -(wallHeight/2. -
                                      photoCathodeThickness/2.)),
                      photoCathode_log,
                      "photocath_cover",
                      scintElementWalls_log,
                      false,
                      0);
}

void BasicDetectorConstruction::placeScintElements()
{
    G4double dx = scintElementWidth+scintElementWallThickness;
    G4double dy = scintElementHeight+scintElementWallThickness;
    // the scintillator elements are not centered at the origin
    // to allow the rays to hit an element in the center
    G4double x = -nx/2*dx + dx*0.75;
    G4double y = -ny/2*dy + dy*0.75;
    G4double z = -70.*cm;
    G4int k = 0;

    for (int j = 0; j < ny; ++j) {
        x = -nx/2*dx + dx*0.75;
        for(int i = 0; i < nx; ++i) {
            new G4PVPlacement(0, G4ThreeVector(x,y,z), scintElementWalls_log,
                              "scint_el", logicWorld, false, k);
            scintElementPositions.push_back(G4ThreeVector(x,y,z));
            x += dx;
            k++;
        }
        y += dy;
    }
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

    G4VisAttributes* photocatheder_va = new G4VisAttributes(G4Colour(0.0,0.75,1.0));
    photoCathode_log->SetVisAttributes(photocatheder_va);

}

//http://refractiveindex.info/?shelf=main&book=CsI&page=Li
// need to review the whole code
void BasicDetectorConstruction::setOpticalProperties()
{
    G4double ephoton[] = {2.07*eV, 2.48*eV};
    const G4int num = sizeof(ephoton)/sizeof(G4double);

    //**Scintillator element properties
    G4double scintElementwalls_reflectivity[] = {0.9, 0.9};
    G4double scintElementwalls_efficiency[] = {0.0, 0.0};
    G4MaterialPropertiesTable* scintElementWallsPT = new G4MaterialPropertiesTable();
    scintElementWallsPT->AddProperty("REFLECTIVITY",
                                     ephoton, scintElementwalls_reflectivity, num);
    scintElementWallsPT->AddProperty("EFFICIENCY", ephoton, scintElementwalls_efficiency, num);
    G4OpticalSurface* scintElementWalls_opsurf =
            new G4OpticalSurface("ScintElementSurface", unified,polished,dielectric_dielectric);
    scintElementWalls_opsurf->SetMaterialPropertiesTable(scintElementWallsPT);

    //**Scintillator element properties
    G4double reflectivity[] = {1.739, 1.739};
    G4double efficiency[] = {0., 0.};
    G4MaterialPropertiesTable* scintElementPT = new G4MaterialPropertiesTable();
    scintElementPT->AddProperty("REFLECTIVITY", ephoton, reflectivity, num);
    scintElementPT->AddProperty("EFFICIENCY", ephoton, efficiency, num);
    G4OpticalSurface* scintElement_opsurf =
            new G4OpticalSurface("ScintElementSurface", unified,polished,dielectric_metal);
    scintElement_opsurf->SetMaterialPropertiesTable(scintElementPT);

    //**Photocathode surface properties
    G4double photocath_EFF[]={1.,1.}; //Enables 'detection' of photons
    // real and imaginary refraction index
    G4double photocath_RINDEX[]={0.78,1.14};
    G4MaterialPropertiesTable* photocath_mt = new G4MaterialPropertiesTable();
    photocath_mt->AddProperty("EFFICIENCY", ephoton, photocath_EFF, num);
    photocath_mt->AddProperty("RINDEX", ephoton, photocath_RINDEX, num);
    G4OpticalSurface* photocath_opsurf=
            new G4OpticalSurface("photocath_opsurf", glisur,polished,dielectric_metal);
    photocath_opsurf->SetMaterialPropertiesTable(photocath_mt);

    //**Create logical skin surfaces
    new G4LogicalSkinSurface("photocath_surf", scintElement_log, scintElement_opsurf);
    new G4LogicalSkinSurface("photocath_surf", photoCathode_log, photocath_opsurf);
    new G4LogicalSkinSurface("walls_surf", scintElementWalls_log, scintElementWalls_opsurf);
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
    photoCathodeThickness = 20.*mm;
    // scintitllator is made of a 150X150 grid
    nx = 150;
    ny = 150;
}
