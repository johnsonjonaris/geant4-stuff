#include "ScintHit.hh"

G4ThreadLocal G4Allocator<ScintHit>* scintHitAllocator=0;

ScintHit::ScintHit() :
    scintNumber(-1),
    nPhotons(0),
    Edep(0.),
    Pos(0.),
    PhysVol(0) {}

ScintHit::ScintHit(G4VPhysicalVolume* pVol) :
    PhysVol(pVol) {}


ScintHit::~ScintHit() {}

ScintHit::ScintHit(const ScintHit &right) : G4VHit()
{
    nPhotons = right.nPhotons;
    scintNumber = right.scintNumber;
    Edep = right.Edep;
    Pos = right.Pos;
    PhysVol = right.PhysVol;
}

const ScintHit& ScintHit::operator=(const ScintHit &right)
{
    nPhotons = right.nPhotons;
    scintNumber = right.scintNumber;
    Edep = right.Edep;
    Pos = right.Pos;
    PhysVol = right.PhysVol;
    return *this;
}

G4int ScintHit::operator==(const ScintHit& right) const{
    return (scintNumber == right.scintNumber);
}

