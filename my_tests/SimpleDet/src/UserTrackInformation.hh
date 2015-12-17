#ifndef UserTrackInformation_h
#define UserTrackInformation_h 1

#include "G4VUserTrackInformation.hh"
#include "globals.hh"


enum TrackStatus { active=1, // still being tracked
                   hitPMT=2, // stopped by being detected in a PMT
                   absorbed=4, // stopped by being absorbed with G4OpAbsorbtion
                   boundaryAbsorbed=8, // stopped by being aborbed with G4OpAbsorbtion
                   inactive=14 // track is stopped for some reason
                   // -This is the sum of all stopped flags so can be used to remove stopped flags
                 };

class UserTrackInformation : public G4VUserTrackInformation
{
public:

    UserTrackInformation() : fStatus(active),fReflections(0),fForcedraw(false) {}
    virtual ~UserTrackInformation() {}

    //Sets the track status to s (does not check validity of flags)
    void SetTrackStatusFlags(int s){fStatus=s;}
    //Does a smart add of track status flags (disabling old flags that conflict)
    //If s conflicts with itself it will not be detected
    void AddTrackStatusFlag(int s);

    int GetTrackStatus()const {return fStatus;}

    void IncReflections(){fReflections++;}
    G4int GetReflectionCount()const {return fReflections;}

    void SetForceDrawTrajectory(G4bool b){fForcedraw=b;}
    G4bool GetForceDrawTrajectory(){return fForcedraw;}

    inline virtual void Print() const{};

private:

    int fStatus;
    G4int fReflections;
    G4bool fForcedraw;
};

#endif
