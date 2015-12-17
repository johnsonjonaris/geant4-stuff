#ifndef SteppingAction_H
#define SteppingACtion_H 1

#include "G4SteppingManager.hh"
#include "G4SDManager.hh"
#include "G4EventManager.hh"
#include "G4ProcessManager.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4Event.hh"
#include "G4StepPoint.hh"
#include "G4TrackStatus.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "globals.hh"
#include "G4UserSteppingAction.hh"
#include "G4OpBoundaryProcess.hh"

#include "EventAction.hh"
#include "TrackingAction.hh"
#include "Trajectory.hh"
#include "PhotoCathodeHit.hh"
#include "PhotoCathodeSD.hh"
#include "UserTrackInformation.hh"
#include "UserEventInformation.hh"

class EventAction;

class SteppingAction : public G4UserSteppingAction
{
public:

    SteppingAction() : fOneStepPrimaries(false), fExpectedNextStatus(Undefined) {}
    virtual ~SteppingAction() {}
    virtual void UserSteppingAction(const G4Step*);

    void SetOneStepPrimaries(G4bool b){fOneStepPrimaries=b;}
    G4bool GetOneStepPrimaries(){return fOneStepPrimaries;}

private:

    G4bool fOneStepPrimaries;
    G4OpBoundaryProcessStatus fExpectedNextStatus;
};

#endif
