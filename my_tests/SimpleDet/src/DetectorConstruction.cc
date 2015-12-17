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
        photoCathodeSD.Get()->initScintillatorElements(nx, ny);
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

    //Polystyrene
    fH = new G4Element("H", "H", 1., 1.01*g/mole);
    fC = new G4Element("C", "C", 6., 12.01*g/mole);

    Pstyrene = new G4Material("Polystyrene", 1.03*g/cm3, 2);
    Pstyrene->AddElement(fC, 8);
    Pstyrene->AddElement(fH, 8);

    CsI = nist->FindOrBuildMaterial("G4_CESIUM_IODIDE");
    G4Element* Tl = nist->FindOrBuildElement("Tl");
    CsI_Tl = new G4Material("CsI_Tl",4.51*g/cm3,2);
    CsI_Tl->AddMaterial(CsI,99.6*perCent);
    CsI_Tl->AddElement(Tl,0.4*perCent);

    Cu = nist->FindOrBuildMaterial("G4_Cu");

    //Glass
    Glass = new G4Material("Glass", 1.032*g/cm3,2);
    Glass->AddElement(fC,91.533*perCent);
    Glass->AddElement(fH,8.467*perCent);

    Al = nist->FindOrBuildMaterial("G4_Al");

    defineMTP();
}

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
void BasicDetectorConstruction::defineMTP()
{
    // create material tables
    // air
    G4double air_Energy[]={1.9*eV,7.0*eV,7.14*eV};
    const G4int airnum = sizeof(air_Energy)/sizeof(G4double);
    G4double air_RIND[]={1.,1.,1.};
    G4MaterialPropertiesTable *air_MPT = new G4MaterialPropertiesTable();
    air_MPT->AddProperty("RINDEX", air_Energy, air_RIND,airnum);
    air->SetMaterialPropertiesTable(air_MPT);
    // ScI Tl
    // source: http://hypernews.slac.stanford.edu/HyperNews/geant4/get/AUX/2011/06/01/03.27-76314-2DetectorConstruction.txt
    const G4int nEntries = 5;
    //energy values; currently this value corresponds to the maximum emission energy of CsI (550 nm)
    // wavelength(nm) = 1240/eV = {449.1126  498.9940  549.1585  598.7446  648.8749}
    G4double photonEnergies[] = {2.761*eV, 2.485*eV, 2.258*eV, 2.071*eV, 1.911*eV};
    // optical properties of Cesium Iodide
    G4double refractiveIndexCsI[] = {1.824, 1.807, 1.795, 1.786, 1.779};
    G4double absorptionLengthCsI[] = {39.3*cm, 39.3*cm, 39.3*cm, 39.3*cm, 39.3*cm};
    G4double Scnt_SLOW[] = { 0.29, 0.75, 1.0, 0.72, 0.39};
    G4MaterialPropertiesTable* CsI_MTP = new G4MaterialPropertiesTable();
    // add the optical properties of Cesium Iodide to this object
    CsI_MTP->AddProperty("FASTCOMPONENT", photonEnergies, Scnt_SLOW, nEntries);
    CsI_MTP->AddProperty("SLOWCOMPONENT", photonEnergies, Scnt_SLOW, nEntries);
    CsI_MTP->AddProperty("RINDEX", photonEnergies, refractiveIndexCsI, nEntries);
    CsI_MTP->AddProperty("ABSLENGTH", photonEnergies, absorptionLengthCsI, nEntries);
    CsI_MTP->AddConstProperty("SCINTILLATIONYIELD", 54000./MeV);
    CsI_MTP->AddConstProperty("YIELDRATIO", 0.0);
    CsI_MTP->AddConstProperty("FASTTIMECONSTANT", 1.*ns);
    CsI_MTP->AddConstProperty("SLOWTIMECONSTANT", 1000.*ns);
    CsI_MTP->AddConstProperty("RESOLUTIONSCALE", 1.0);
    CsI_Tl->SetMaterialPropertiesTable(CsI_MTP);
    // not sure of the value
    CsI_Tl->GetIonisation()->SetBirksConstant(0.126*mm/MeV);

    // Polystyrene
    // does that need modification
    // source: examples/extended/optical/LXe
    G4double rIndexPstyrene[] = {1.6, 1.6, 1.6, 1.6, 1.6};
    G4double absorption1[] = {2.*cm, 2.*cm, 2.*cm, 2.*cm, 2.*cm};
    //G4double scintilFast[] = { 1., 1., 1. , 1., 1.};
    G4MaterialPropertiesTable *PStyrene_MTP = new G4MaterialPropertiesTable();
    PStyrene_MTP->AddProperty("RINDEX", photonEnergies, rIndexPstyrene, nEntries);
    PStyrene_MTP->AddProperty("ABSLENGTH", photonEnergies, absorption1, nEntries);
    //PStyrene_MTP->AddProperty("FASTCOMPONENT", photonEnergies, scintilFast, nEntries);
    //PStyrene_MTP->AddConstProperty("SCINTILLATIONYIELD", 10./keV);
    //PStyrene_MTP->AddConstProperty("RESOLUTIONSCALE", 1.0);
    //PStyrene_MTP->AddConstProperty("FASTTIMECONSTANT", 10.*ns);
    Pstyrene->SetMaterialPropertiesTable(PStyrene_MTP);
    Pstyrene->GetIonisation()->SetBirksConstant(0.126*mm/MeV);

    // Glass
    G4double glass_RIND[]={1.49, 1.49, 1.49, 1.49, 1.49};
    G4double glass_AbsLength[]={420.*cm, 420.*cm, 420.*cm, 420.*cm, 420.*cm};
    G4MaterialPropertiesTable *glass_MPT = new G4MaterialPropertiesTable();
    glass_MPT->AddProperty("ABSLENGTH",photonEnergies,glass_AbsLength,nEntries);
    glass_MPT->AddProperty("RINDEX",photonEnergies,glass_RIND,nEntries);
    Glass->SetMaterialPropertiesTable(glass_MPT);
    /*
    // Aluminum
    G4double refractiveIndexAl[] = {0.61367, 0.78129, 0.96175, 1.1342, 1.3332};
    //http://physics.nist.gov/PhysRefData/XrayMassCoef/ElemTab/z13.html
    G4double absorptionLengthAl[] = {320*m,320*m,320*m,320*m,320*m};
    G4MaterialPropertiesTable* Al_MTP = new G4MaterialPropertiesTable();
    Al_MTP->AddProperty("RINDEX",photonEnergies,refractiveIndexAl,nEntries);
    Al_MTP->AddProperty("ABSLENGTH",photonEnergies,absorptionLengthAl,nEntries);
    Al->SetMaterialPropertiesTable(Al_MTP);

    // copper
    G4double refractiveIndexCu[] = {1.32, 1.07, 0.68, 0.42, 0.27};
    //http://physics.nist.gov/PhysRefData/XrayMassCoef/ElemTab/z29.html
    G4double absorptionLengthCu[] = {947*m,947*m,947*m,947*m,947*m};
    G4MaterialPropertiesTable* Cu_MTP = new G4MaterialPropertiesTable();
    Cu_MTP->AddProperty("RINDEX",photonEnergies,refractiveIndexCu,nEntries);
    Cu_MTP->AddProperty("ABSLENGTH",photonEnergies,absorptionLengthCu,nEntries);
    Cu->SetMaterialPropertiesTable(Cu_MTP);
    */
}


void BasicDetectorConstruction::defineScintillatorElement()
{
    /*
        define scintillator element
        the element is a box made of CsI and of 40 mm thickness
        its walls (0.05 um thickness) are covered by Polystyrene
        the top is covered by a copper layer of 1 mm thickness
        the back is covered by a pmt glass layer of 1 mm
        the glass layer is followd by an aluminum layer of 10 mm
        thickness that will absorb the photons

          1mm       40 mm   1mm 20 mm
          |-|---------------|-|---|
          | |---------------| |   |
          | |      _|_      | |   |
          | |       |       | |   |
          | |---------------| |   |
          |-|---------------|-|---|
      */
    G4double dx = scintElementSide+scintElementWallThickness;
    G4double dy = scintElementSide+scintElementWallThickness;
    scintElement_box = new G4Box("scint_box",
                                 scintElementSide/2.,
                                 scintElementSide/2.,
                                 scintElementHeight/2.);
    scintElementWalls_box = new G4Box("walls_box",
                                      dx/2., dy/2., scintElementHeight/2.);
    pmt_box = new G4Box("pmt_box",
                        dx/2., dy/2., pmtHeight/2.);

    photoCathode_box = new G4Box("photocath_box",
                                 dx/2., dy/2., photoCathodeHeight/2.);
    G4double detectorThickness =
            scintElementCoverThickness +
            scintElementHeight +
            pmtHeight +
            photoCathodeHeight +
            10.*mm;
    detectorHousing_box = new G4Box("housing_box",
                                    (nx*dx + 2*detectorHousingThickness)/2.,
                                    (ny*dy + 2*detectorHousingThickness)/2.,
                                    detectorThickness/2.);

    scintElement_log = new G4LogicalVolume(scintElement_box, CsI_Tl, "scint_log",0,0,0);
    scintElementWalls_log = new G4LogicalVolume(scintElementWalls_box, Pstyrene, "walls_log",0,0,0);
    // the housing will act as the 1 mm cover layer
    detectorHousing_log = new G4LogicalVolume(detectorHousing_box, Cu, "housing_log", 0,0,0);
    //the "photocathode" is a metal slab at the back of the glass that
    //is only a very rough approximation of the real thing since it only
    //absorbs or detects the photons based on the efficiency set below
    pmt_log = new G4LogicalVolume(pmt_box, Glass, "pmt_log",0,0,0);
    photoCathode_log = new G4LogicalVolume(photoCathode_box, Al, "photocath_log",0,0,0);

    // put CsI_Tl scintillator in the walls
    new G4PVPlacement(0,                                    // no rotation
                      G4ThreeVector(0.,0.,0.),          // translation
                      scintElement_log,                            // the logical volume
                      "scint",                              // name
                      scintElementWalls_log,                            // mother volume
                      false,                                // no boolean operation
                      0);                                   // its copy number

    // put Al photocathode on the back of pmt
    new G4PVPlacement(0,
                      G4ThreeVector(0.,0.,
                                    -pmtHeight/2.-photoCathodeHeight/2.),
                      photoCathode_log,
                      "photocathode",
                      pmt_log,
                      false,
                      0);

    // put Glass pmt on the back of scintillator
    new G4PVPlacement(0,
                      G4ThreeVector(0.,0.,
                                    -scintElementHeight/2.-pmtHeight/2.),
                      pmt_log,
                      "pmt",
                      scintElementWalls_log,
                      false,
                      0);
    // put housing
    // put Glass pmt on the back of scintillator
    new G4PVPlacement(0,
                      G4ThreeVector(-dx/2.,-dy/2.,-70.*cm),
                      detectorHousing_log,
                      "housing",
                      logicWorld,
                      false,
                      0);

}

void BasicDetectorConstruction::placeScintElements()
{
    G4double dx = scintElementSide+scintElementWallThickness;
    G4double dy = scintElementSide+scintElementWallThickness;
    // the scintillator elements are not centered at the origin
    // to allow the rays to hit an element in the center
    G4double x;
    G4double y = -ny/2.*dy + dy/2.;
    G4double z = 4.*mm;
    G4int k = 0;

    for (int j = 0; j < ny; ++j) {
        x = -nx/2.*dx + dx/2.;
        for(int i = 0; i < nx; ++i) {
            new G4PVPlacement(0, G4ThreeVector(x,y,z), scintElementWalls_log,
                              "scint_el", detectorHousing_log, false, k);
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
    scintElementWalls_log->SetVisAttributes(walls_va);

    G4VisAttributes* pmt_va = new G4VisAttributes(G4Colour(1.,0.5,0.0));
    pmt_log->SetVisAttributes(pmt_va);

    G4VisAttributes* photocatheder_va = new G4VisAttributes(G4Colour(0.0,0.75,1.0));
    photoCathode_log->SetVisAttributes(photocatheder_va);

    G4VisAttributes* housing_va = new G4VisAttributes(G4Colour(1.,0.,0.));
    detectorHousing_log->SetVisAttributes(housing_va);
}

//http://refractiveindex.info/?shelf=main&book=CsI&page=Li
// need to review the whole code
void BasicDetectorConstruction::setOpticalProperties()
{
    G4double ephoton[] = {2.761*eV,2.485*eV,2.258*eV,2.071*eV,1.911*eV};
    // wavelength = {449.1126  498.9940  549.1585  598.7446  648.8749}
    const G4int num = sizeof(ephoton)/sizeof(G4double);

    // CsI - Polystrene edge
    // reflectivity from Wang's paper
    // the main use of the walls is to reflect the beams inside the element
    G4double scintElementwalls_reflectivity[] = {0.9, 0.9, 0.9, 0.9, 0.9};
    G4double scintElementwalls_efficiency[] = {0.1, 0.1, 0.1, 0.1, 0.1};
    G4MaterialPropertiesTable* scintElementWallsPT = new G4MaterialPropertiesTable();
    scintElementWallsPT->AddProperty("REFLECTIVITY", ephoton, scintElementwalls_reflectivity, num);
    scintElementWallsPT->AddProperty("EFFICIENCY", ephoton, scintElementwalls_efficiency, num);
    G4OpticalSurface* scintElementWalls_opsurf =
            new G4OpticalSurface("ScintElementSurface", unified, polished, dielectric_dielectric);
    scintElementWalls_opsurf->SetMaterialPropertiesTable(scintElementWallsPT);

    //**Photocathode surface properties
    G4double photocath_efficiency[]={1., 1., 1., 1., 1.}; //'detection' efficiency of photons
    // real and imaginary refraction index
    G4double photocath_ReR[]={1.92,1.92,1.92,1.92,1.92};
    G4double photocath_ImR[]={1.69,1.69,1.69,1.69,1.69};
    G4MaterialPropertiesTable* photocath_mt = new G4MaterialPropertiesTable();
    photocath_mt->AddProperty("EFFICIENCY", ephoton, photocath_efficiency, num);
    photocath_mt->AddProperty("REALRINDEX",ephoton,photocath_ReR,num);
    photocath_mt->AddProperty("IMAGINARYRINDEX",ephoton,photocath_ImR,num);
    G4OpticalSurface* photocath_opsurf=
            new G4OpticalSurface("photocath_opsurf", glisur, polished, dielectric_metal);
    photocath_opsurf->SetMaterialPropertiesTable(photocath_mt);

    // extend housing photon range
    G4double housing_efficiency[] = {1.,1.,1.,1.,1.};
    G4double housing_reflectivity[] = {0.65, 0.65, 0.65, 0.65, 0.65};
    //http://www.filmetrics.com/refractive-index-database/Cu/Copper
    G4double housing_ReR[]={1.16,1.13,0.95,0.3513,0.214};
    G4double housing_ImR[]={2.39,2.56,2.57,3.01,3.67};
    G4MaterialPropertiesTable* scintHsngPT = new G4MaterialPropertiesTable();
    scintHsngPT->AddProperty("REFLECTIVITY", ephoton, housing_reflectivity, num);
    scintHsngPT->AddProperty("EFFICIENCY", ephoton, housing_efficiency, num);
    scintHsngPT->AddProperty("REALRINDEX",ephoton,housing_ReR,num);
    scintHsngPT->AddProperty("IMAGINARYRINDEX",ephoton,housing_ImR,num);
    G4OpticalSurface* housing_opsurf=
            new G4OpticalSurface("photocath_opsurf", glisur, polished, dielectric_metal);
    housing_opsurf->SetMaterialPropertiesTable(scintHsngPT);

    //**Create logical skin surfaces
    new G4LogicalSkinSurface("walls_surf", scintElementWalls_log, scintElementWalls_opsurf);
    new G4LogicalSkinSurface("photocath_surf", photoCathode_log, photocath_opsurf);
    new G4LogicalSkinSurface("housing_surf", detectorHousing_log, housing_opsurf);
}

void BasicDetectorConstruction::setDefaultValues()
{
    // values according to: Wang et. al. "High-DQE EPIDs based on thick, segmented BGO
    // and CsI:Tl scintillators: Performance evaluation at extremely low dose"
    scintElementSide = 1.016*mm;
    scintElementHeight = 40.0*mm;
    scintElementWallThickness = 0.05*mm;
    scintElementCoverThickness = 1.0*mm;
    detectorHousingThickness = 4.*mm;
    pmtHeight = 1.0*mm;
    photoCathodeHeight = 4.*mm;
    // scintitllator is made of a 160X160 grid
    nx = 160;
    ny = 160;
}
