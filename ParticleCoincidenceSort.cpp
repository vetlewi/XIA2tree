//
// Created by Vetle Wegner Ingeberg on 07/10/2022.
//

#include <UserSort/UserSort.h>
#include <histogram/Histogram1D.h>
#include <histogram/Histogram2D.h>
#include <histogram/Histograms.h>
#include <histogram/ThreadSafeHistograms.h>

#include "event.h"

class ParticleConicidence : public UserSort
{
private:
    ThreadSafeHistogram1D trap_mult[8];
    ThreadSafeHistogram2D ede[8][8];

public:
    ParticleConicidence(ThreadSafeHistograms *hist);
    void FillEvent(const Triggered_event &event) override;
    void Flush() override;

};

extern "C" {
    UserSort *NewUserSort(ThreadSafeHistograms *hist){
        return new ParticleConicidence(hist);
    }
}

ParticleConicidence::ParticleConicidence(ThreadSafeHistograms *hist)
    : trap_mult{ hist->Create1D("trap_mult_b0", "trap_mult_b0", 10, 0, 10, "Hits"),
                 hist->Create1D("trap_mult_b1", "trap_mult_b1", 10, 0, 10, "Hits"),
                 hist->Create1D("trap_mult_b2", "trap_mult_b2", 10, 0, 10, "Hits"),
                 hist->Create1D("trap_mult_b3", "trap_mult_b3", 10, 0, 10, "Hits"),
                 hist->Create1D("trap_mult_b4", "trap_mult_b4", 10, 0, 10, "Hits"),
                 hist->Create1D("trap_mult_b5", "trap_mult_b5", 10, 0, 10, "Hits"),
                 hist->Create1D("trap_mult_b6", "trap_mult_b6", 10, 0, 10, "Hits"),
                 hist->Create1D("trap_mult_b7", "trap_mult_b7", 10, 0, 10, "Hits")}
    , ede{{hist->Create2D("ede_b0_f0", "ede_b0_f0", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b0_f1", "ede_b0_f1", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b0_f2", "ede_b0_f2", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b0_f3", "ede_b0_f3", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b0_f4", "ede_b0_f4", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b0_f5", "ede_b0_f5", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b0_f6", "ede_b0_f6", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b0_f7", "ede_b0_f7", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]")},
          {hist->Create2D("ede_b1_f0", "ede_b1_f0", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b1_f1", "ede_b1_f1", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b1_f2", "ede_b1_f2", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b1_f3", "ede_b1_f3", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b1_f4", "ede_b1_f4", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b1_f5", "ede_b1_f5", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b1_f6", "ede_b1_f6", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b1_f7", "ede_b1_f7", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]")},
          {hist->Create2D("ede_b2_f0", "ede_b2_f0", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b2_f1", "ede_b2_f1", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b2_f2", "ede_b2_f2", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b2_f3", "ede_b2_f3", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b2_f4", "ede_b2_f4", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b2_f5", "ede_b2_f5", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b2_f6", "ede_b2_f6", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b2_f7", "ede_b2_f7", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]")},
          {hist->Create2D("ede_b3_f0", "ede_b3_f0", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b3_f1", "ede_b3_f1", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b3_f2", "ede_b3_f2", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b3_f3", "ede_b3_f3", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b3_f4", "ede_b3_f4", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b3_f5", "ede_b3_f5", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b3_f6", "ede_b3_f6", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b3_f7", "ede_b3_f7", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]")},
          {hist->Create2D("ede_b4_f0", "ede_b4_f0", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b4_f1", "ede_b4_f1", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b4_f2", "ede_b4_f2", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b4_f3", "ede_b4_f3", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b4_f4", "ede_b4_f4", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b4_f5", "ede_b4_f5", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b4_f6", "ede_b4_f6", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b4_f7", "ede_b4_f7", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]")},
          {hist->Create2D("ede_b5_f0", "ede_b5_f0", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b5_f1", "ede_b5_f1", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b5_f2", "ede_b5_f2", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b5_f3", "ede_b5_f3", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b5_f4", "ede_b5_f4", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b5_f5", "ede_b5_f5", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b5_f6", "ede_b5_f6", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b5_f7", "ede_b5_f7", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]")},
          {hist->Create2D("ede_b6_f0", "ede_b6_f0", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b6_f1", "ede_b6_f1", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b6_f2", "ede_b6_f2", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b6_f3", "ede_b6_f3", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b6_f4", "ede_b6_f4", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b6_f5", "ede_b6_f5", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b6_f6", "ede_b6_f6", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b6_f7", "ede_b6_f7", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]")},
          {hist->Create2D("ede_b7_f0", "ede_b7_f0", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b7_f1", "ede_b7_f1", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b7_f2", "ede_b7_f2", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b7_f3", "ede_b7_f3", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b7_f4", "ede_b7_f4", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b7_f5", "ede_b7_f5", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b7_f6", "ede_b7_f6", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]"),
           hist->Create2D("ede_b7_f7", "ede_b7_f7", 1000, 0, 16384, "E energy [ch]", 1000, 0, 16384, "DE energy [ch]")}}
{}
/*{
    char tmp[128];
    for ( int i = 0 ; i < 8 ; ++i ){
        sprintf(tmp, "trap_mult_b%d", i);
        //trap_mult[i] = hist->Create1D(tmp, tmp, 10, 0, 10, "Hits");
        for ( int j = 0 ; j < 8 ; ++j ){
            sprintf(tmp, "ede_b%d_f%d", i, j);
            ede[i][j] = hist->Create2D(tmp, tmp, 1000, 0, 16384, "E energy [ch]",
                                                             1000, 0, 16384, "DE energy [ch]");
        }
    }
}*/

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
            trap_mult[i].Fill(trap.size());
        if ( trap.size() == 1 ){
            ede[trap[0].first.detectorID][trap[0].second.detectorID % NUM_SI_DE_TEL].Fill(trap[0].first.energy, trap[0].second.energy);
        }
        i++;
        if ( trap.size() == 2 ){
            continue;
        }
    }

}

void ParticleConicidence::Flush()
{
    for ( auto &trap : trap_mult ){
        trap.force_flush();
    }
    for ( auto &trap : ede ){
        for ( auto &ring : trap ){
            ring.force_flush();
        }
    }
}