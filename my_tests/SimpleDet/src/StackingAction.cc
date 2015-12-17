#include "StackingAction.hh"

G4ClassificationOfNewTrack
StackingAction::ClassifyNewTrack(const G4Track * aTrack){

    UserEventInformation* eventInformation=
            (UserEventInformation*)G4EventManager::GetEventManager()
            ->GetConstCurrentEvent()->GetUserInformation();

    //Count what process generated the optical photons
    if(aTrack->GetDefinition()==G4OpticalPhoton::OpticalPhotonDefinition()){
        // particle is optical photon
        if(aTrack->GetParentID()>0){
            // particle is secondary
            if(aTrack->GetCreatorProcess()->GetProcessName()=="Scintillation")
                eventInformation->IncPhotonCount_Scint();
            else if(aTrack->GetCreatorProcess()->GetProcessName()=="Cerenkov")
                eventInformation->IncPhotonCount_Ceren();
        }
    }
    else{
    }
    return fUrgent;
}
