#include "TrackingAction.hh"

void TrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
    //Let this be up to the user via vis.mac
    //  fpTrackingManager->SetStoreTrajectory(true);

    //Use custom trajectory class
    fpTrackingManager->SetTrajectory(new Trajectory(aTrack));

    //This user track information is only relevant to the photons
    fpTrackingManager->SetUserTrackInformation(new UserTrackInformation());
}

void TrackingAction::PostUserTrackingAction(const G4Track* aTrack){
    Trajectory* trajectory=(Trajectory*)fpTrackingManager->GimmeTrajectory();
    UserTrackInformation*
            trackInformation=(UserTrackInformation*)aTrack->GetUserInformation();

    //Lets choose to draw only the photons that hit the pmt
    if(aTrack->GetDefinition()==G4OpticalPhoton::OpticalPhotonDefinition()){

        const G4VProcess* creator=aTrack->GetCreatorProcess();
        if(creator && creator->GetProcessName()=="OpWLS"){
            trajectory->WLS();
            trajectory->SetDrawTrajectory(true);
        }

        if(trackInformation->GetTrackStatus()&hitPMT) {
            trajectory->SetDrawTrajectory(true);
        }
    }
    else //draw all other trajectories
        trajectory->SetDrawTrajectory(true);

    if(trackInformation->GetForceDrawTrajectory())
        trajectory->SetDrawTrajectory(true);
}
