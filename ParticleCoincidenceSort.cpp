//
// Created by Vetle Wegner Ingeberg on 07/10/2022.
//

#include "Sort.h"

#include <histogram/Histogram1D.h>
#include <histogram/Histogram2D.h>
#include <histogram/Histograms.h>

class ParticleConicidence : public Task::UserSort
{
private:
    Histogram1Dp trap_mult[8];
    Histogram2Dp ede[8][8];

public:
    ParticleConicidence(Histograms *hist);
    void FillEvent(const Triggered_event &event) override;

};

extern "C" {
    Task::UserSort *NewUserSort(Histograms *hist){
        return new ParticleConicidence(hist);
    }
}

ParticleConicidence::ParticleConicidence(Histograms *hist)
{
    char tmp[128];
    for ( int i = 0 ; i < 8 ; ++i ){
        sprintf(tmp, "trap_mult_b%d", i);
        trap_mult[i] = hist->Create1D(tmp, tmp, 10, 0, 10, "Hits");
        for ( int j = 0 ; j < 8 ; ++j ){
            sprintf(tmp, "ede_b%d_f%d", i, j);
            ede[i][j] = hist->Create2D(tmp, tmp, 1000, 0, 16384, "E energy [ch]",
                                                             1000, 0, 16384, "DE energy [ch]");
        }
    }
}

void ParticleConicidence::FillEvent(const Triggered_event &event)
{
    std::vector<std::pair<Entry_t, Entry_t>> particle_hits[8];
    for ( auto &e : event.GetDetector(eDet) ){
        for ( auto de : event.GetDetector(deDet) ){
            if ( e.detectorID == (de.detectorID / NUM_SI_DE_TEL) ) {
                particle_hits[e.detectorID].emplace_back(std::make_pair(e, de));
            }
        }
    }

    // Now we can count how many entries there are in each trapezoid
    int i = 0;
    for ( auto &trap : particle_hits ){
        if ( !trap.empty() )
            trap_mult[i]->Fill(trap.size());
        if ( trap.size() == 1 ){
            ede[trap[0].first.detectorID][trap[0].second.detectorID % NUM_SI_DE_TEL]->Fill(trap[0].first.energy, trap[0].second.energy);
        }
        i++;
        if ( trap.size() == 2 ){
            continue;
        }
    }

}