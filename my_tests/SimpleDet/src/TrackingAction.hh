#ifndef TrackingAction_h
#define TrackingAction_h 1

#include "G4UserTrackingAction.hh"
#include "globals.hh"
#include "G4TrackingManager.hh"
#include "G4Track.hh"
#include "G4ParticleTypes.hh"

#include "Trajectory.hh"
#include "UserTrackInformation.hh"
#include "DetectorConstruction.hh"


class TrackingAction : public G4UserTrackingAction {

public:

    TrackingAction() {}
    virtual ~TrackingAction() {};

    virtual void PreUserTrackingAction(const G4Track*);
    virtual void PostUserTrackingAction(const G4Track*);
};

#endif
