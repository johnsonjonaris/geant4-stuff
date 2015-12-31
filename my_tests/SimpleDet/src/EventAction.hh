#ifndef EventAction_h
#define EventAction_h 1

#include "G4EventManager.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4VVisManager.hh"
#include "G4ios.hh"
#include "G4UImanager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UserEventAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"

#include "ScintHit.hh"
#include "PhotoCathodeHit.hh"
#include "PhotoCathodeSD.hh"
#include "UserEventInformation.hh"
#include "Trajectory.hh"

#include "fstream"

using namespace std;

class G4Event;

class EventAction : public G4UserEventAction
{
public:

    EventAction() : fSaveThreshold(0),fScintCollID(-1),fPMTCollID(-1),fVerbose(0),
        fPMTThreshold(1),fForcedrawphotons(false),fForcenophotons(false)
    {}
    virtual ~EventAction() {}

public:

    virtual void BeginOfEventAction(const G4Event*);
    virtual void EndOfEventAction(const G4Event*);

    void SetSaveThreshold(G4int );

    void SetEventVerbose(G4int v){fVerbose=v;}

    void SetPMTThreshold(G4int t){fPMTThreshold=t;}

    void SetForceDrawPhotons(G4bool b){fForcedrawphotons=b;}
    void SetForceDrawNoPhotons(G4bool b){fForcenophotons=b;}

private:

    G4int              fSaveThreshold;

    G4int              fScintCollID;
    G4int              fPMTCollID;

    G4int              fVerbose;

    G4int              fPMTThreshold;

    G4bool fForcedrawphotons;
    G4bool fForcenophotons;

    void saveImage(const G4Event *anEvent);
};

#endif
