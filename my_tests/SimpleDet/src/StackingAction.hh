#ifndef StackingAction_H
#define StackingAction_H 1

#include "G4ios.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "globals.hh"
#include "G4UserStackingAction.hh"

#include "UserEventInformation.hh"
#include "SteppingAction.hh"

class StackingAction : public G4UserStackingAction
{
public:

    StackingAction() {}
    virtual ~StackingAction() {}

    virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);
    virtual void NewStage() {}
    virtual void PrepareNewEvent() {}

};

#endif
