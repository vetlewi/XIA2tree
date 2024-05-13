//
// Created by Vetle Wegner Ingeberg on 06/12/2022.
//

#include <UserSort/UserSort.h>
#include <histogram/ThreadSafeHistograms.h>
#include <Format/event.h>

class Co60CoincidenceSort : public UserSort
{
private:
    ThreadSafeHistogram3D gamgam;
    ThreadSafeHistogram2D time_labr_fine;
    ThreadSafeHistogram1D timediff_de;

    int64_t last_ts;
    double last_cfd;
public:
    Co60CoincidenceSort(ThreadSafeHistograms *hist);
    void FillEvent(const Triggered_event &event) override;
    void Flush() override;
};


extern "C" {
    UserSort *NewUserSort(ThreadSafeHistograms *hist){
        return new Co60CoincidenceSort(hist);
    }
}

Co60CoincidenceSort::Co60CoincidenceSort(ThreadSafeHistograms *hist)
    : gamgam(hist->Create3D("gamgam", "Gamma - gamma time matrix",
                            30, 0, 30, "Start",
                            30, 0, 30, "Stop",
                            1000, -100, 100, "Time [ns]"))
    , time_labr_fine(hist->Create2D("time_labr_fine", "Timespectrum labr",
                                    20000, -100, 100, "Time [ns]",
                                    30, 0, 30, "LaBr ID"))
    , timediff_de( hist->Create1D("timediff_de", "time difference between two consecutive dE events", 9000, 0, 90000, "Time [ns]") )
    {}

void Co60CoincidenceSort::FillEvent(const Triggered_event &event)
{
    // Get all the ede events
    auto hits_de = event.GetDetector(DetectorType::deDet);
    for ( size_t i = 1 ; i < hits_de.size() ; ++i ){
        double tdiff = (hits_de[i]->timestamp - hits_de[i-1]->timestamp) + (hits_de[i]->cfdcorr - hits_de[i]->cfdcorr);
        timediff_de.Fill(tdiff);
    }
    double tdiff = (hits_de[0]->timestamp - last_ts) + (hits_de[0]->cfdcorr - last_cfd);
    timediff_de.Fill(tdiff);
    last_ts = hits_de[hits_de.size()-1]->timestamp;
    last_cfd = hits_de[hits_de.size()-1]->cfdcorr;
    // Get all the labr hits
    auto hits = event.GetDetector(DetectorType::labr);
    const Entry_t *trigger = event.GetTrigger();
    for ( size_t i = 0 ; i < hits.size() ; ++i ){
        if ( trigger ) {
            double timediff = double(hits[i]->timestamp - trigger->timestamp) + (hits[i]->cfdcorr - trigger->cfdcorr);
            time_labr_fine.Fill(timediff, hits[i]->detectorID);
        }

        for ( size_t j = 0 ; j < hits.size() ; ++j ){
            if ( i == j )
                continue;

            double timediff = double( hits[j]->timestamp - hits[i]->timestamp ) + (hits[j]->cfdcorr - hits[i]->cfdcorr);
            gamgam.Fill(hits[i]->detectorID, hits[j]->detectorID, timediff);
        }
    }

}

void Co60CoincidenceSort::Flush()
{
    gamgam.force_flush();
    time_labr_fine.force_flush();
    timediff_de.force_flush();
}