#include "ScintHit.hh"

G4ThreadLocal G4Allocator<ScintHit>* scintHitAllocator=0;

ScintHit::ScintHit() :
    Edep(0.),
    Pos(0.),
    PhysVol(0) {}

ScintHit::ScintHit(G4VPhysicalVolume* pVol) :
    PhysVol(pVol) {}


ScintHit::~ScintHit() {}

ScintHit::ScintHit(const ScintHit &right) : G4VHit()
{
    Edep = right.Edep;
    Pos = right.Pos;
    PhysVol = right.PhysVol;
}

const ScintHit& ScintHit::operator=(const ScintHit &right)
{
    Edep = right.Edep;
    Pos = right.Pos;
    PhysVol = right.PhysVol;
    return *this;
}

G4int ScintHit::operator==(const ScintHit& right) const{
    return false;
}

