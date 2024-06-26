//
// Created by Vetle Wegner Ingeberg on 07/10/2022.
//

#include <UserSort/UserSort.h>
#include <histogram/Histogram1D.h>
#include <histogram/Histogram2D.h>
#include <histogram/Histograms.h>
#include <histogram/ThreadSafeHistograms.h>
#include <PhysicalParam/ConfigManager.h>
#include <PhysicalParam/ParticleRange.h>
#include <Format/event.h>

#include <zstr.hpp>

#include <fstream>
#include <sstream>
#include <thread>


class ParticleConicidence : public UserSort
{
private:
    ParticleRange range;
private:
    ThreadSafeHistogram1D trap_mult[8];
    ThreadSafeHistogram2D ede[8][8];
    ThreadSafeHistogram2D ede_tot;
    ThreadSafeHistogram2D ede_tot_strip;
    ThreadSafeHistogram2D ede_cut[8];
    ThreadSafeHistogram2D rangeHist;
    ThreadSafeHistogram2D excitation_singles;
    ThreadSafeHistogram2D excitationCoincidence;
    ThreadSafeHistogram2D excitationBackground;
    //ThreadSafeHistogram3D exgam_time;

    ThreadSafeHistogram2D gamgam_time;
    ThreadSafeHistogram1D gamma_gamma_time;
    ThreadSafeHistogram2D prompt_timing_by_detector[30];
    ThreadSafeHistogram2D gamma_time_energy;
    ThreadSafeHistogram2D gamma_time;
    ThreadSafeHistogram2D gamma_time1st0pGated;
    ThreadSafeHistogram2D gamma_1st0pGated_bgSubtr;

public:
    ParticleConicidence(ThreadSafeHistograms *hist);
    ~ParticleConicidence() = default;
    void FillEvent(const Triggered_event &event) override;
    void Flush() override;

};

extern "C" {
    UserSort *NewUserSort(ThreadSafeHistograms *hist, const OCL::ConfigManager &config){
        return new ParticleConicidence(hist);
    }
}

ParticleConicidence::ParticleConicidence(ThreadSafeHistograms *hist)
    : range( "zrange_p.dat" )
    , trap_mult{ hist->Create1D("trap_mult_b0", "trap_mult_b0", 10, 0, 10, "Hits"),
                 hist->Create1D("trap_mult_b1", "trap_mult_b1", 10, 0, 10, "Hits"),
                 hist->Create1D("trap_mult_b2", "trap_mult_b2", 10, 0, 10, "Hits"),
                 hist->Create1D("trap_mult_b3", "trap_mult_b3", 10, 0, 10, "Hits"),
                 hist->Create1D("trap_mult_b4", "trap_mult_b4", 10, 0, 10, "Hits"),
                 hist->Create1D("trap_mult_b5", "trap_mult_b5", 10, 0, 10, "Hits"),
                 hist->Create1D("trap_mult_b6", "trap_mult_b6", 10, 0, 10, "Hits"),
                 hist->Create1D("trap_mult_b7", "trap_mult_b7", 10, 0, 10, "Hits")}
    , ede{{hist->Create2D("ede_b0_f0", "ede_b0_f0", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b0_f1", "ede_b0_f1", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b0_f2", "ede_b0_f2", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b0_f3", "ede_b0_f3", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b0_f4", "ede_b0_f4", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b0_f5", "ede_b0_f5", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b0_f6", "ede_b0_f6", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b0_f7", "ede_b0_f7", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]")},
          {hist->Create2D("ede_b1_f0", "ede_b1_f0", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b1_f1", "ede_b1_f1", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b1_f2", "ede_b1_f2", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b1_f3", "ede_b1_f3", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b1_f4", "ede_b1_f4", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b1_f5", "ede_b1_f5", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b1_f6", "ede_b1_f6", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b1_f7", "ede_b1_f7", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]")},
          {hist->Create2D("ede_b2_f0", "ede_b2_f0", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b2_f1", "ede_b2_f1", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b2_f2", "ede_b2_f2", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b2_f3", "ede_b2_f3", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b2_f4", "ede_b2_f4", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b2_f5", "ede_b2_f5", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b2_f6", "ede_b2_f6", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b2_f7", "ede_b2_f7", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]")},
          {hist->Create2D("ede_b3_f0", "ede_b3_f0", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b3_f1", "ede_b3_f1", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b3_f2", "ede_b3_f2", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b3_f3", "ede_b3_f3", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b3_f4", "ede_b3_f4", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b3_f5", "ede_b3_f5", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b3_f6", "ede_b3_f6", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b3_f7", "ede_b3_f7", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]")},
          {hist->Create2D("ede_b4_f0", "ede_b4_f0", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b4_f1", "ede_b4_f1", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b4_f2", "ede_b4_f2", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b4_f3", "ede_b4_f3", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b4_f4", "ede_b4_f4", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b4_f5", "ede_b4_f5", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b4_f6", "ede_b4_f6", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b4_f7", "ede_b4_f7", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]")},
          {hist->Create2D("ede_b5_f0", "ede_b5_f0", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b5_f1", "ede_b5_f1", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b5_f2", "ede_b5_f2", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b5_f3", "ede_b5_f3", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b5_f4", "ede_b5_f4", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b5_f5", "ede_b5_f5", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b5_f6", "ede_b5_f6", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b5_f7", "ede_b5_f7", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]")},
          {hist->Create2D("ede_b6_f0", "ede_b6_f0", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b6_f1", "ede_b6_f1", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b6_f2", "ede_b6_f2", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b6_f3", "ede_b6_f3", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b6_f4", "ede_b6_f4", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b6_f5", "ede_b6_f5", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b6_f6", "ede_b6_f6", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b6_f7", "ede_b6_f7", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]")},
          {hist->Create2D("ede_b7_f0", "ede_b7_f0", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b7_f1", "ede_b7_f1", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b7_f2", "ede_b7_f2", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b7_f3", "ede_b7_f3", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b7_f4", "ede_b7_f4", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b7_f5", "ede_b7_f5", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b7_f6", "ede_b7_f6", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
           hist->Create2D("ede_b7_f7", "ede_b7_f7", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]")}}
    , ede_tot( hist->Create2D("ede_tot", "E + #Delta E energy", 1500, 0, 15000, "E + #DeltaE energy [keV]", 64, 0, 64, "RingID") )
    , ede_tot_strip( hist->Create2D("ede_tot_strip", "E + #Delta E energy", 2000, 4000, 16000, "E + #DeltaE energy [keV]", 8, 0, 8, "Ring number") )
    , ede_cut{hist->Create2D("ede_cut_f0", "ede_cut_f0", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
             hist->Create2D("ede_cut_f1", "ede_cut_f1", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
             hist->Create2D("ede_cut_f2", "ede_cut_f2", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
             hist->Create2D("ede_cut_f3", "ede_cut_f3", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
             hist->Create2D("ede_cut_f4", "ede_cut_f4", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
             hist->Create2D("ede_cut_f5", "ede_cut_f5", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
             hist->Create2D("ede_cut_f6", "ede_cut_f6", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]"),
             hist->Create2D("ede_cut_f7", "ede_cut_f7", 1000, 0, 16000, "E energy [keV]", 1000, 0, 6000, "#Delta E energy [keV]")}
    , rangeHist( hist->Create2D("thickDE", "thickDE", 1000, 0, 1000, "#delta E thickness [um]", 64, 0, 64, "Ring ID") )
    , excitation_singles( hist->Create2D("excitation_singles", "Excitation spectrum", 11000, -2000, 20000, "Excitation energy [keV]", 64, 0, 64, "RingID") )
    , excitationCoincidence( hist->Create2D("exgam", "Excitation vs. gamma energy spectrum", 1500, 0, 15000, "LaBr3:Ce energy [keV]", 1700, -2000, 15000, "Excitation energy [keV]") )
    , excitationBackground( hist->Create2D("exgam_bg", "Excitation vs. gamma energy spectrum (bg)", 1500, 0, 15000, "LaBr3:Ce energy [keV]", 1700, -2000, 15000, "Excitation energy [keV]") )
    //, exgam_time( hist->Create3D("exgam_time", "Excitation vs. gamma energy spectrum vs timediff", 1000, 0, 10000, "LaBr3:Ce energy [keV]", 1200, -2000, 10000, "Excitation energy [keV]", 1000, -500, 500, "Time [ns]") )
    , gamgam_time( hist->Create2D("gamgam_time", "Time for gamma-gamma after particle", 1000, -500, 500, "Time of first gamma [ns]", 1000, -500, 500, "Time of second gamma [ns]") )
    , gamma_gamma_time( hist->Create1D("gamma_gamma_time", "Prompt gamma-gamma time", 2000, -10, 10, "Time t_{1779 keV} - t_{2838 keV} [ns]") )
    , gamma_time_energy( hist->Create2D("gamma_time_energy", "Time vs energy", 3000, 0, 3000, "Energy [keV]", 1000, -50, 50, "t_{E_\gamma} - t_{1779 keV} [ns]") )
    , gamma_time( hist->Create2D("gamma_time", "Time energy spectrum", 1500, 0, 15000, "Energy [keV]", 1000, -500, 500, "Time [ns]") )
    , gamma_time1st0pGated( hist->Create2D("gamma_time1st0pGated", "Time energy spectrum, gated on 1st 0+", 1500, 0, 15000, "Energy [keV]", 4000, -500, 500, "Time [ns]") )
    , gamma_1st0pGated_bgSubtr( hist->Create2D("gamma_1st0pGated_bgSubtr", "Time energy spectrum, gated on 1st 0+", 5000, 0, 5000, "Energy [keV]", 2, 0, 2, "Prompt/background") )
    , prompt_timing_by_detector{
        hist->Create2D("prompt_timing_by_detector_00", "prompt_timing_by_detector_00", 1000, -20, 20, "Time [ns]", 30, 0, 30, "Stop ID"),
        hist->Create2D("prompt_timing_by_detector_01", "prompt_timing_by_detector_01", 1000, -20, 20, "Time [ns]", 30, 0, 30, "Stop ID"),
        hist->Create2D("prompt_timing_by_detector_02", "prompt_timing_by_detector_02", 1000, -20, 20, "Time [ns]", 30, 0, 30, "Stop ID"),
        hist->Create2D("prompt_timing_by_detector_03", "prompt_timing_by_detector_03", 1000, -20, 20, "Time [ns]", 30, 0, 30, "Stop ID"),
        hist->Create2D("prompt_timing_by_detector_04", "prompt_timing_by_detector_04", 1000, -20, 20, "Time [ns]", 30, 0, 30, "Stop ID"),
        hist->Create2D("prompt_timing_by_detector_05", "prompt_timing_by_detector_05", 1000, -20, 20, "Time [ns]", 30, 0, 30, "Stop ID"),
        hist->Create2D("prompt_timing_by_detector_06", "prompt_timing_by_detector_06", 1000, -20, 20, "Time [ns]", 30, 0, 30, "Stop ID"),
        hist->Create2D("prompt_timing_by_detector_07", "prompt_timing_by_detector_07", 1000, -20, 20, "Time [ns]", 30, 0, 30, "Stop ID"),
        hist->Create2D("prompt_timing_by_detector_08", "prompt_timing_by_detector_08", 1000, -20, 20, "Time [ns]", 30, 0, 30, "Stop ID"),
        hist->Create2D("prompt_timing_by_detector_09", "prompt_timing_by_detector_09", 1000, -20, 20, "Time [ns]", 30, 0, 30, "Stop ID"),
        hist->Create2D("prompt_timing_by_detector_10", "prompt_timing_by_detector_10", 1000, -20, 20, "Time [ns]", 30, 0, 30, "Stop ID"),
        hist->Create2D("prompt_timing_by_detector_11", "prompt_timing_by_detector_11", 1000, -20, 20, "Time [ns]", 30, 0, 30, "Stop ID"),
        hist->Create2D("prompt_timing_by_detector_12", "prompt_timing_by_detector_12", 1000, -20, 20, "Time [ns]", 30, 0, 30, "Stop ID"),
        hist->Create2D("prompt_timing_by_detector_13", "prompt_timing_by_detector_13", 1000, -20, 20, "Time [ns]", 30, 0, 30, "Stop ID"),
        hist->Create2D("prompt_timing_by_detector_14", "prompt_timing_by_detector_14", 1000, -20, 20, "Time [ns]", 30, 0, 30, "Stop ID"),
        hist->Create2D("prompt_timing_by_detector_15", "prompt_timing_by_detector_15", 1000, -20, 20, "Time [ns]", 30, 0, 30, "Stop ID"),
        hist->Create2D("prompt_timing_by_detector_16", "prompt_timing_by_detector_16", 1000, -20, 20, "Time [ns]", 30, 0, 30, "Stop ID"),
        hist->Create2D("prompt_timing_by_detector_17", "prompt_timing_by_detector_17", 1000, -20, 20, "Time [ns]", 30, 0, 30, "Stop ID"),
        hist->Create2D("prompt_timing_by_detector_18", "prompt_timing_by_detector_18", 1000, -20, 20, "Time [ns]", 30, 0, 30, "Stop ID"),
        hist->Create2D("prompt_timing_by_detector_19", "prompt_timing_by_detector_19", 1000, -20, 20, "Time [ns]", 30, 0, 30, "Stop ID"),
        hist->Create2D("prompt_timing_by_detector_20", "prompt_timing_by_detector_20", 1000, -20, 20, "Time [ns]", 30, 0, 30, "Stop ID"),
        hist->Create2D("prompt_timing_by_detector_21", "prompt_timing_by_detector_21", 1000, -20, 20, "Time [ns]", 30, 0, 30, "Stop ID"),
        hist->Create2D("prompt_timing_by_detector_22", "prompt_timing_by_detector_22", 1000, -20, 20, "Time [ns]", 30, 0, 30, "Stop ID"),
        hist->Create2D("prompt_timing_by_detector_23", "prompt_timing_by_detector_23", 1000, -20, 20, "Time [ns]", 30, 0, 30, "Stop ID"),
        hist->Create2D("prompt_timing_by_detector_24", "prompt_timing_by_detector_24", 1000, -20, 20, "Time [ns]", 30, 0, 30, "Stop ID"),
        hist->Create2D("prompt_timing_by_detector_25", "prompt_timing_by_detector_25", 1000, -20, 20, "Time [ns]", 30, 0, 30, "Stop ID"),
        hist->Create2D("prompt_timing_by_detector_26", "prompt_timing_by_detector_26", 1000, -20, 20, "Time [ns]", 30, 0, 30, "Stop ID"),
        hist->Create2D("prompt_timing_by_detector_27", "prompt_timing_by_detector_27", 1000, -20, 20, "Time [ns]", 30, 0, 30, "Stop ID"),
        hist->Create2D("prompt_timing_by_detector_28", "prompt_timing_by_detector_28", 1000, -20, 20, "Time [ns]", 30, 0, 30, "Stop ID"),
        hist->Create2D("prompt_timing_by_detector_29", "prompt_timing_by_detector_29", 1000, -20, 20, "Time [ns]", 30, 0, 30, "Stop ID"),
    }
{}

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
    std::pair<Entry_t, Entry_t> particle_event;
    for ( auto &trap : particle_hits ){
        if ( trap.empty() )
            continue;
        trap_mult[i].Fill(trap.size());
        if ( trap.size() == 1 ){
            ede[trap[0].first.detectorID][trap[0].second.detectorID % NUM_SI_DE_TEL].Fill(trap[0].first.energy, trap[0].second.energy);
        }
        i += trap.size();
        particle_event = trap[0];
        if ( trap.size() == 2 ){
            continue;
        }
    }

    // We can only analyze events with only one trapezoid hit.
    if ( i != 1 )
        return;

    auto [eEvent, deEvent] = particle_event;

    if ( eEvent.cfdfail || deEvent.cfdfail )
        return;

    //if ( eEvent.detectorID == 0 && deEvent.detectorID == 0 )
    //    ede_table << eEvent.adcvalue << "," << deEvent.adcvalue << "\n";
    //hits.emplace_back(eEvent.detectorID, deEvent.detectorID, eEvent.adcvalue, deEvent.adcvalue);

    // If we get here, particle_event will be the only "event" that occured.
    double thickness = range(eEvent.energy+deEvent.energy) - range(eEvent.energy);
    rangeHist.Fill(thickness, deEvent.detectorID);

    if ( thickness > 110 && thickness < 150 ){
        int ringNo = deEvent.detectorID % NUM_SI_DE_TEL;
        ede_cut[ringNo].Fill(eEvent.energy, deEvent.energy);
    } else {
        return;
    }

    ede_tot.Fill(eEvent.energy + deEvent.energy, deEvent.detectorID);
    ede_tot_strip.Fill(eEvent.energy + deEvent.energy, deEvent.detectorID % NUM_SI_DE_TEL);

    double ex = 0;//CalcEx(eEvent.energy+deEvent.energy, deEvent.detectorID % NUM_SI_DE_TEL);
    //ex = 1.09275494*ex + 355.18129813;
    excitation_singles.Fill(ex, deEvent.detectorID);

    double timediff = 0;
    for ( auto &gamma : event.GetDetector(DetectorType::labr) ){
        timediff = double(gamma.timestamp - deEvent.timestamp) + double(gamma.cfdcorr - deEvent.cfdcorr);
        if ( gamma.cfdfail )
            continue;
//        exgam_time.Fill(gamma.energy, ex, timediff);
        if ( timediff > -4.9020919 && timediff < 5.0979081 )
            excitationCoincidence.Fill(gamma.energy, ex);
        else if ( timediff > 53.6764 && timediff < 63.6764 )
            excitationBackground.Fill(gamma.energy, ex);
        if ( ex > 4772. && ex < 5200. ) {
                gamma_time1st0pGated.Fill(gamma.energy, timediff);
                if ( timediff > -2 && timediff < 2 )
                    gamma_1st0pGated_bgSubtr.Fill(gamma.energy, 0.5);
                if ( timediff > 56.6 && timediff < 60.7 )
                    gamma_1st0pGated_bgSubtr.Fill(gamma.energy, 1.5);

        }

        if ( ex > 1516.4336 && ex < 2039.6464 ){
            int ringNo = gamma.detectorID % 6;
            //gamma_1779keV.Fill(gamma.energy, timediff, ringNo);
        }


    }

    auto labr_tmp = event.GetDetector(DetectorType::labr);
    std::vector<Entry_t> labrEvts;
    for ( auto &l : labr_tmp ){ labrEvts.push_back(l); }
    std::sort(labrEvts.begin(), labrEvts.end(), [](const Entry_t &lhs, const Entry_t &rhs)
    { return (double(rhs.timestamp - lhs.timestamp) + (rhs.cfdcorr - lhs.cfdcorr)) > 0; });

    for ( size_t n = 0 ; n < labrEvts.size() ; ++n ){
        if ( labrEvts[n].cfdfail )
            continue;
        double tdiff0 = double(labrEvts[n].timestamp - deEvent.timestamp) + (labrEvts[n].cfdcorr - deEvent.cfdcorr);
        gamma_time.Fill(labrEvts[n].energy, timediff);
        for ( size_t m = 0 ; m < labrEvts.size() ; ++m ){
            if ( m == n ) // Skip same gamma
                continue;
        //for ( size_t m = n+1 ; m < labr_tmp.size() ; ++m ){
            if ( labrEvts[m].cfdfail )
                continue;
            double tdiff1 = double(labrEvts[m].timestamp - deEvent.timestamp) + (labrEvts[m].cfdcorr - deEvent.cfdcorr);

            if ( ex > 4200 && ex < 5600 ) {
                gamgam_time.Fill(tdiff0, tdiff1);

                if (!( (tdiff0 > -10 && tdiff0 < 10) &&
                     (tdiff1 > -10 && tdiff1 < 10))){
                    continue;
                }

                if ( (labrEvts[n].energy > 1750 && labrEvts[n].energy < 1830) ){
                    gamma_time_energy.Fill(labrEvts[m].energy, double(labrEvts[m].timestamp - labrEvts[n].timestamp) + labrEvts[m].cfdcorr - labrEvts[n].cfdcorr);
                    prompt_timing_by_detector[labrEvts[n].detectorID].Fill(double(labrEvts[m].timestamp - labrEvts[n].timestamp) + labrEvts[m].cfdcorr - labrEvts[n].cfdcorr,
                                                                           labrEvts[m].detectorID);
                }

                if ( (labrEvts[n].energy > 1750 && labrEvts[n].energy < 1830) &&
                     (labrEvts[m].energy > 2800 && labrEvts[m].energy < 2900) ){
                    gamma_gamma_time.Fill(double(labrEvts[m].timestamp - labrEvts[n].timestamp) + labrEvts[m].cfdcorr - labrEvts[n].cfdcorr);
                }


            }
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
    for ( auto &det : ede_cut ){
        det.force_flush();
    }
    rangeHist.force_flush();
    ede_tot.force_flush();
    ede_tot_strip.force_flush();
    excitation_singles.force_flush();
    excitationCoincidence.force_flush();
    excitationBackground.force_flush();
    //exgam_time.force_flush();
    gamma_time.force_flush();
    gamma_gamma_time.force_flush();
    gamgam_time.force_flush();
    gamma_time1st0pGated.force_flush();
    //gamma_1779keV.force_flush();
}