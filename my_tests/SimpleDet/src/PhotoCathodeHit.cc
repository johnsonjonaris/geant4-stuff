#include "PhotoCathodeHit.hh"

G4ThreadLocal G4Allocator<PhotoCathodeHit>* pmtHitAllocator=0;

PhotoCathodeHit::PhotoCathodeHit() :
    scintNumber(-1),
    nPhotons(0),
    drawIt(false),
    PhysVol(0) {}

PhotoCathodeHit::PhotoCathodeHit(G4VPhysicalVolume* pVol) :
    PhysVol(pVol) {}


PhotoCathodeHit::~PhotoCathodeHit() {}

PhotoCathodeHit::PhotoCathodeHit(const PhotoCathodeHit &right) : G4VHit()
{
    nPhotons = right.nPhotons;
    scintNumber = right.scintNumber;
    drawIt = right.drawIt;
    Pos = right.Pos;
    PhysVol = right.PhysVol;
}

const PhotoCathodeHit& PhotoCathodeHit::operator=(const PhotoCathodeHit &right)
{
    nPhotons = right.nPhotons;
    scintNumber = right.scintNumber;
    drawIt = right.drawIt;
    Pos = right.Pos;
    PhysVol = right.PhysVol;
    return *this;
}

G4int PhotoCathodeHit::operator==(const PhotoCathodeHit& right) const{
    return (scintNumber == right.scintNumber);
}

