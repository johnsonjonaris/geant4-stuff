#include "EventAction.hh"

void EventAction::BeginOfEventAction(const G4Event* anEvent){

    //New event, add the user information object
    G4EventManager::
            GetEventManager()->SetUserInformation(new UserEventInformation);

    G4SDManager* SDman = G4SDManager::GetSDMpointer();
    if(fScintCollID<0)
        fScintCollID=SDman->GetCollectionID("scintCollection");
    if(fPMTCollID<0)
        fPMTCollID=SDman->GetCollectionID("pmtHitCollection");
}

void EventAction::EndOfEventAction(const G4Event* anEvent){

    UserEventInformation* eventInformation
            =(UserEventInformation*)anEvent->GetUserInformation();

    G4TrajectoryContainer* trajectoryContainer=anEvent->GetTrajectoryContainer();

    G4int n_trajectories = 0;
    if (trajectoryContainer) n_trajectories = trajectoryContainer->entries();

    // extract the trajectories and draw them
    if (G4VVisManager::GetConcreteInstance()){
        for (G4int i=0; i<n_trajectories; i++){
            Trajectory* trj = (Trajectory*)
                    ((*(anEvent->GetTrajectoryContainer()))[i]);
            if(trj->GetParticleName()=="opticalphoton"){
                trj->SetForceDrawTrajectory(fForcedrawphotons);
                trj->SetForceNoDrawTrajectory(fForcenophotons);
            }
            trj->DrawTrajectory();
        }
    }

    ScintHitsCollection* scintHC = 0;
    PhotoCathodeHitsCollection* pmtHC = 0;
    G4HCofThisEvent* hitsCE = anEvent->GetHCofThisEvent();

    //Get the hit collections
    if(hitsCE){
        if(fScintCollID>=0) scintHC = (ScintHitsCollection*)(hitsCE->GetHC(fScintCollID));
        if(fPMTCollID>=0) pmtHC = (PhotoCathodeHitsCollection*)(hitsCE->GetHC(fPMTCollID));
    }

    //Hits in scintillator
    if(scintHC){
        int n_hit = scintHC->entries();
        G4ThreeVector  eWeightPos(0.);
        G4double edep;
        G4double edepMax=0;

        for(int i=0;i<n_hit;i++){ //gather info on hits in scintillator
            edep=(*scintHC)[i]->GetEdep();
            eventInformation->IncEDep(edep); //sum up the edep
            eWeightPos += (*scintHC)[i]->GetPos()*edep;//calculate energy weighted pos
            if(edep>edepMax){
                edepMax=edep;//store max energy deposit
                G4ThreeVector posMax=(*scintHC)[i]->GetPos();
                eventInformation->SetPosMax(posMax,edep);
            }
        }
        if(eventInformation->GetEDep()!=0.){
            //Finish calculation of energy weighted position
            eWeightPos/=eventInformation->GetEDep();
            eventInformation->SetEWeightPos(eWeightPos);
        }
    }

    if(pmtHC){
        G4ThreeVector reconPos(0.,0.,0.);
        G4int pmts=pmtHC->entries();
        //Gather info from all PMTs
        for(G4int i=0;i<pmts;i++){
            eventInformation->IncHitCount((*pmtHC)[i]->GetPhotonCount());
            reconPos+=(*pmtHC)[i]->GetPos()*(*pmtHC)[i]->GetPhotonCount();
            if((*pmtHC)[i]->GetPhotonCount()>=fPMTThreshold){
                eventInformation->IncPMTSAboveThreshold();
            }
            else{//wasnt above the threshold, turn it back off
                (*pmtHC)[i]->SetDrawit(false);
            }
        }

        if(eventInformation->GetHitCount()>0){//dont bother unless there were hits
            reconPos/=eventInformation->GetHitCount();
            eventInformation->SetReconPos(reconPos);
        }
        pmtHC->DrawAllHits();
    }
    //If we have set the flag to save 'special' events, save here
    if(fSaveThreshold&&eventInformation->GetPhotonCount() <= fSaveThreshold)
        G4RunManager::GetRunManager()->rndmSaveThisEvent();

    saveImage(anEvent);
    printf("Event done\n");
}

void EventAction::saveImage(const G4Event* anEvent)
{
    // save an image
    PhotoCathodeSD *pmtSD = (PhotoCathodeSD*)G4SDManager::GetSDMpointer()
            ->FindSensitiveDetector("PhotoCathodeSD");

    PhotoCathodeHitsCollection* pmtHC = 0;
    G4HCofThisEvent* hitsCE = anEvent->GetHCofThisEvent();

    //Get the hit collections
    if(hitsCE){
        if(fPMTCollID>=0) pmtHC = (PhotoCathodeHitsCollection*)(hitsCE->GetHC(fPMTCollID));
    }
    if(!pmtSD || !pmtHC) {
        return;
    }

    G4int nx, ny;
    pmtSD->getArraySize(nx, ny);
    //uchar_mat image = zeros<uchar_mat>(nx,ny);
    unsigned char *image = new unsigned char[nx*ny];
    for (int i=0; i<nx*ny; i++)
        image[i] = 0;
    if(pmtHC) {
        G4int pmts=pmtHC->entries();
        for(int i=0; i< pmts; i++) {
            G4int pmtNumber = (*pmtHC)[i]->GetNumber();
            G4int nHits = (*pmtHC)[i]->GetPhotonCount();
            int xi = pmtNumber % nx;
            int yi = (pmtNumber/nx) % ny;
            //printf("pmtN %d = (%d,%d), nHits %d\n", pmtNumber, xi, yi, nHits);
            image[pmtNumber] = nHits;
        }
        // save the image
        G4int eventNum = anEvent->GetEventID();
        printf("event number %d\n", eventNum);
        G4String filename = "D:/Workspace/results/result_" + G4String(std::to_string(eventNum));
        ofstream myFile (filename, ios::out | ios::binary);
        myFile.write((char*)image, nx*ny);
    }
    delete image;
}

void EventAction::SetSaveThreshold(G4int save){
    /*Sets the save threshold for the random number seed. If the number of photons
generated in an event is lower than this, then save the seed for this event
in a file called run###evt###.rndm
*/
    fSaveThreshold=save;
    G4RunManager::GetRunManager()->SetRandomNumberStore(true);
    G4RunManager::GetRunManager()->SetRandomNumberStoreDir("random/");
}
