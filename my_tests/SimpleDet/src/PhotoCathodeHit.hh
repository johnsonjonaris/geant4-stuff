#ifndef PHOTOCATHODEHIT_H
#define PHOTOCATHODEHIT_H 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ios.hh"
#include "G4VVisManager.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include "tls.hh"

class PhotoCathodeHit : public G4VHit
{
public:

    PhotoCathodeHit();
    PhotoCathodeHit(G4VPhysicalVolume* pVol);
    virtual ~PhotoCathodeHit();
    PhotoCathodeHit(const PhotoCathodeHit &right);
    const PhotoCathodeHit& operator=(const PhotoCathodeHit &right);
    G4int operator==(const PhotoCathodeHit &right) const;

    inline void *operator new(size_t);
    inline void operator delete(void *aHit);

    virtual void Draw() {}
    virtual void Print() {}

    inline void SetDrawit(G4bool b){drawIt=b;}
    inline G4bool GetDrawit(){return drawIt;}

    inline void IncPhotonCount(){ nPhotons++; }
    inline G4int GetPhotonCount() const { return nPhotons; }

    inline void SetNumber(G4int n) { scintNumber = n; }
    inline G4int GetNumber() const { return scintNumber; }

    inline void SetPos(G4ThreeVector xyz) { Pos = xyz; }
    inline void SetPos(G4double x,G4double y,G4double z)
    { Pos = G4ThreeVector(x,y,z); }
    inline G4ThreeVector GetPos() const { return Pos; }

    inline void SetPhysVol(G4VPhysicalVolume* physVol){ this->PhysVol=physVol; }
    inline G4VPhysicalVolume * GetPhysV() { return PhysVol; }

private:
    G4int nPhotons;
    G4int scintNumber;
    G4ThreeVector Pos;
    G4VPhysicalVolume* PhysVol;
    G4bool drawIt;
};

typedef G4THitsCollection<PhotoCathodeHit> PhotoCathodeHitsCollection;

extern G4ThreadLocal G4Allocator<PhotoCathodeHit>* pmtHitAllocator;

inline void* PhotoCathodeHit::operator new(size_t)
{
    if(!pmtHitAllocator)
        pmtHitAllocator = new G4Allocator<PhotoCathodeHit>;
    return (void *) pmtHitAllocator->MallocSingle();
}

inline void PhotoCathodeHit::operator delete(void *aHit)
{
    pmtHitAllocator->FreeSingle((PhotoCathodeHit*) aHit);
}

#endif
