//
// Created by Vetle Wegner Ingeberg on 20/10/2022.
//

// TODO:
/*
 * 1) Make ede_spectra_bxfy always fill -> good check that data are correctly calibrated
 * 2) Make ede_spectra_gated_fx -> To see with and without the gate
 * 3) Need to be able to pass time gate and
 */

#include "MTSort.h"
#include "TTreeManager.h"
#include "ParticleRange.h"
#include "ConfigManager.h"

using namespace Task;


// For 59Co
/*constexpr double a2[] = {0.000123, 0.000097, 0.000070, 0.000040, 0.000008, -0.000025, -0.000061, -0.000097};
constexpr double a1[] = {-1.033612, -1.032484, -1.031292, -1.030005, -1.028671, -1.027260, -1.025767, -1.024225};
constexpr double a0[] = {15.482874, 15.482970, 15.482859, 15.482356, 15.481656, 15.480574, 15.479027, 15.477125};*/

// For 106Pd
/*constexpr double a2[] = {-0.000004, -0.000020, -0.000038, -0.000057, -0.000077, -0.000099, -0.000121, -0.000146};
constexpr double a1[] = {-1.016472, -1.015782, -1.015050, -1.014245, -1.013415, -1.012532, -1.011625, -1.010621};
constexpr double a0[] = {15.680808, 15.680103, 15.679198, 15.677920, 15.676482, 15.674706, 15.672703, 15.670059};*/

// For 117Sn
constexpr double a2[] = { 0.000126,  0.000115,  0.000103,  0.000091,  0.000078,  0.000065,  0.000050,  0.000035};
constexpr double a1[] = {-1.018782, -1.018272, -1.017734, -1.017168, -1.016573, -1.015948, -1.015291, -1.014599};
constexpr double a0[] = {15.759522, 15.760104, 15.760629, 15.761086, 15.761454, 15.761709, 15.761821, 15.761756};

constexpr double CalcEx(const size_t& ringNo, const double& p_energy) {
    return a0[ringNo] + a1[ringNo] * p_energy + a2[ringNo] * p_energy * p_energy;
}

Detector_Histograms_t::Detector_Histograms_t(ThreadSafeHistograms &hm, const std::string &name, const size_t &num)
    : time( hm.Create2D(std::string("time_"+name), std::string("Time spectra "+name), 30000, -1500, 1500, "Time [ns]", num, 0, num, std::string(name+" ID")) )
    , time_CFDfail( hm.Create2D(std::string("time_"+name+"_CFDfail"), std::string("Time spectra"+name+" CFD fail"), 30000, -1500, 1500, "Time [ns]", num, 0, num, std::string(name+" ID")) )
    , energy( hm.Create2D(std::string("energy_"+name), std::string("Energy spectra "+name), 65536, 0, 65536, "Energy [ch]", num, 0, num, std::string(name+" ID")) )
    , energy_cal( hm.Create2D(std::string("energy_cal_"+name), std::string("energy spectra "+name+" (cal)"), 16384, 0, 16384, "Energy [keV]", num, 0, num, std::string(name+" ID")) )
    , mult( hm.Create1D(std::string("mult_"+name), std::string("Multiplicity " + name), 128, 0, 128, "Multiplicity") )
{}

void Detector_Histograms_t::Fill(const Entry_t &word)
{
    energy.Fill(word.adcvalue, word.detectorID);
    energy_cal.Fill(word.energy, word.detectorID);
}

void Detector_Histograms_t::Fill(const subvector<Entry_t> &subvec,
                                 const Entry_t *start)
{
    mult.Fill(subvec.size());
    for ( auto &entry : subvec ){
        energy.Fill(entry.adcvalue, entry.detectorID);
        energy_cal.Fill(entry.energy, entry.detectorID);

        if ( start && !( (entry.type == start->type)&&(entry.detectorID == start->detectorID)&&(entry.timestamp==start->timestamp)) ){
            if ( entry.cfdfail )
                time_CFDfail.Fill(double(entry.timestamp - start->timestamp) + (entry.cfdcorr - start->cfdcorr), entry.detectorID + 0.5);
            else
                time.Fill(double(entry.timestamp - start->timestamp) + (entry.cfdcorr - start->cfdcorr), entry.detectorID + 0.5);
        }
    }
}

void Detector_Histograms_t::Flush()
{
    time.force_flush();
    energy.force_flush();
    energy_cal.force_flush();
    mult.force_flush();
}

Particle_telescope_t::Particle_telescope_t(ThreadSafeHistograms &hm, const size_t &num)
        : ede_spectra{hm.Create2D("ede_spectra_b"+std::to_string(num)+"f0", "E energy vs dE energy",
                                  3000, 0, 30000, "E energy [keV]", 1500, 0, 15000, "dE energy [keV]"),
                      hm.Create2D("ede_spectra_b"+std::to_string(num)+"f1", "E energy vs dE energy",
                                  3000, 0, 30000, "E energy [keV]", 1500, 0, 15000, "dE energy [keV]"),
                      hm.Create2D("ede_spectra_b"+std::to_string(num)+"f2", "E energy vs dE energy",
                                  3000, 0, 30000, "E energy [keV]", 1500, 0, 15000, "dE energy [keV]"),
                      hm.Create2D("ede_spectra_b"+std::to_string(num)+"f3", "E energy vs dE energy",
                                  3000, 0, 30000, "E energy [keV]", 1500, 0, 15000, "dE energy [keV]"),
                      hm.Create2D("ede_spectra_b"+std::to_string(num)+"f4", "E energy vs dE energy",
                                  3000, 0, 30000, "E energy [keV]", 1500, 0, 15000, "dE energy [keV]"),
                      hm.Create2D("ede_spectra_b"+std::to_string(num)+"f5", "E energy vs dE energy",
                                  3000, 0, 30000, "E energy [keV]", 1500, 0, 15000, "dE energy [keV]"),
                      hm.Create2D("ede_spectra_b"+std::to_string(num)+"f6", "E energy vs dE energy",
                                  3000, 0, 30000, "E energy [keV]", 1500, 0, 15000, "dE energy [keV]"),
                      hm.Create2D("ede_spectra_b"+std::to_string(num)+"f7", "E energy vs dE energy",
                                  3000, 0, 30000, "E energy [keV]", 2048, 0, 16384, "dE energy [keV]")}
        , ede_spectra_raw{ hm.Create2D("ede_spectra_raw_b"+std::to_string(num)+"f0", "E energy vs dE energy (raw, uncalibrated)",
                                   2048, 0, 16384, "E energy [ch]", 2048, 0, 16384, "dE energy [ch]"),
                           hm.Create2D("ede_spectra_raw_b"+std::to_string(num)+"f1", "E energy vs dE energy (raw, uncalibrated)",
                                   2048, 0, 16384, "E energy [ch]", 2048, 0, 16384, "dE energy [ch]"),
                           hm.Create2D("ede_spectra_raw_b"+std::to_string(num)+"f2", "E energy vs dE energy (raw, uncalibrated)",
                                   2048, 0, 16384, "E energy [ch]", 2048, 0, 16384, "dE energy [ch]"),
                           hm.Create2D("ede_spectra_raw_b"+std::to_string(num)+"f3", "E energy vs dE energy (raw, uncalibrated)",
                                   2048, 0, 16384, "E energy [ch]", 2048, 0, 16384, "dE energy [ch]"),
                           hm.Create2D("ede_spectra_raw_b"+std::to_string(num)+"f4", "E energy vs dE energy (raw, uncalibrated)",
                                   2048, 0, 16384, "E energy [ch]", 2048, 0, 16384, "dE energy [ch]"),
                           hm.Create2D("ede_spectra_raw_b"+std::to_string(num)+"f5", "E energy vs dE energy (raw, uncalibrated)",
                                   2048, 0, 16384, "E energy [ch]", 2048, 0, 16384, "dE energy [ch]"),
                           hm.Create2D("ede_spectra_raw_b"+std::to_string(num)+"f6", "E energy vs dE energy (raw, uncalibrated)",
                                   2048, 0, 16384, "E energy [ch]", 2048, 0, 16384, "dE energy [ch]"),
                           hm.Create2D("ede_spectra_raw_b"+std::to_string(num)+"f7", "E energy vs dE energy (raw, uncalibrated)",
                                   2048, 0, 16384, "E energy [ch]", 2048, 0, 16384, "dE energy [ch]")}
{
}

void Particle_telescope_t::Fill(const std::vector<Entry_t> &deltaE, const std::vector<Entry_t> &E)
//void Particle_telescope_t::Fill(const subvector<Entry_t> &deltaE, const subvector<Entry_t> &E)
{
    for ( auto &de : deltaE ){
        for ( auto &e : E ){

            // Check if the same trapzoid. If not, then we will just skip.
            if ( de.detectorID / NUM_SI_DE_TEL != e.detectorID )
                continue;

            ede_spectra[de.detectorID % NUM_SI_DE_TEL].Fill(e.energy, de.energy);
            ede_spectra_raw[de.detectorID % NUM_SI_DE_TEL].Fill(e.adcvalue, de.adcvalue);
        }
    }
}

void Particle_telescope_t::Flush()
{
    for ( auto &ede : ede_spectra_raw ){
        ede.force_flush();
    }
    for ( auto &ede : ede_spectra )
        ede.force_flush();
}


HistManager::HistManager(ThreadSafeHistograms &histograms, const OCL::UserConfiguration &user_config, const char *custom_sort)
        : configuration( user_config )
        , labr( histograms, "labr", NUM_LABR_DETECTORS )
        , si_de( histograms, "si_de", NUM_SI_DE_DET )
        , si_e( histograms, "si_e", NUM_SI_E_DET )
        , ppacs( histograms, "ppac", NUM_PPAC )
        , particle_coincidence{{ histograms, 0},
                               { histograms, 1},
                               { histograms, 2},
                               { histograms, 3},
                               { histograms, 4},
                               { histograms, 5},
                               { histograms, 6},
                               { histograms, 7}}
        , ede_spectra{ histograms.Create2D("ede_spectra_f0", "Particle identification, ring 0",
                                           2500, 0, 25000, "E energy [keV]",
                                           1500, 0, 15000, "#Delta E energy [keV]"),
                       histograms.Create2D("ede_spectra_f1", "Particle identification, ring 1",
                                           2500, 0, 25000, "E energy [keV]",
                                           1500, 0, 15000, "#Delta E energy [keV]"),
                       histograms.Create2D("ede_spectra_f2", "Particle identification, ring 2",
                                           2500, 0, 25000, "E energy [keV]",
                                           1500, 0, 15000, "#Delta E energy [keV]"),
                       histograms.Create2D("ede_spectra_f3", "Particle identification, ring 3",
                                           2500, 0, 25000, "E energy [keV]",
                                           1500, 0, 15000, "#Delta E energy [keV]"),
                       histograms.Create2D("ede_spectra_f4", "Particle identification, ring 4",
                                           2500, 0, 25000, "E energy [keV]",
                                           1500, 0, 15000, "#Delta E energy [keV]"),
                       histograms.Create2D("ede_spectra_f5", "Particle identification, ring 5",
                                           2500, 0, 25000, "E energy [keV]",
                                           1500, 0, 15000, "#Delta E energy [keV]"),
                       histograms.Create2D("ede_spectra_f6", "Particle identification, ring 6",
                                           2500, 0, 25000, "E energy [keV]",
                                           1500, 0, 15000, "#Delta E energy [keV]"),
                       histograms.Create2D("ede_spectra_f7", "Particle identification, ring 7",
                                           2500, 0, 25000, "E energy [keV]",
                                           1500, 0, 15000, "#Delta E energy [keV]"),}
       , ede_time( histograms.Create2D("ede_time", "E time spectrum",
                                       2500, 0, 25000, "E energy [keV]",
                                       1000, -500, 500, "Time [ns]"))
       , thickness( histograms.Create2D("thickness", "dE thickness",
                                        1000, 0, 1000, "dE thickness [keV]",
                                        8, 0, 8, "Ring #"))
       , particle_energy( histograms.Create2D("particle_energy", "Total particle energy",
                                              16384, 0, 16384, "Etot(Ede+Ee) [keV]",
                                              8, 0, 8, "Ring #") )
       , alfna_prompt( histograms.Create2D("alfna_prompt", "Particle - gamma coincidence",
                                            1500, 0, 15000, "E gamma [keV]",
                                            1800, -2000, 15000, "Excitation [keV]") )
       , alfna_background( histograms.Create2D("alfna_background", "Particle - gamma coincidence",
                                                        1500, 0, 15000, "E gamma [keV]",
                                                        1800, -2000, 15000, "Excitation [keV]") )
       , ts_ex_above_Sn( histograms.Create2D("ts_ex_above_Sn", "Time spectra (LaBr) above Sn",
                                             30000, -1500, 1500, "Time [ns]",
                                             NUM_LABR_DETECTORS, 0, NUM_LABR_DETECTORS, "Detector ID") )
       , mult_ex( histograms.Create2D("mult_ex", "Multiplicity as function of Ex (± 10 ns)",
                                                 15000, 0, 15000, "Excitation energy [keV]",
                                                    20, 0, 20, "Multiplicity") )
       , userSort( histograms, configuration, custom_sort )
{
}

Detector_Histograms_t *HistManager::GetSpec(const DetectorType &type)
{
    switch ( type ) {
        case DetectorType::labr : return &labr;
        case DetectorType::deDet : return &si_de;
        case DetectorType::eDet : return &si_e;
        case DetectorType::ppac : return &ppacs;
        default : return nullptr;
    }
}

void HistManager::AddEntry(Triggered_event &buffer)
{
    auto trigger = buffer.GetTrigger();

    // For now, we will discard events with bad CFD
    // We have this req. if we get a trigger
    if ( trigger )
        if ( trigger->cfdfail )
            return;

    for ( auto &type : {DetectorType::labr, DetectorType::deDet, DetectorType::eDet, DetectorType::ppac} ){
        GetSpec(type)->Fill(buffer.GetDetector(type), trigger);
    }

    auto trapID = trigger->detectorID / 8;
    auto ringID = trigger->detectorID % 8;
    auto [de_evts, e_evts] = buffer.GetTrap(trapID);
    GetPart(trapID)->Fill(de_evts, e_evts);

    if ( de_evts.size() != 1 || e_evts.size() != 1 )
        return; // DONE!!

    auto e_evt = e_evts[0];

    //double etot = e_evt->energy + trigger->energy;
    double etot = e_evt.energy + trigger->energy;

    //double thick = configuration.GetRange().GetRange(etot) - configuration.GetRange().GetRange(e_evts[0]->energy);
    double thick = configuration.GetRange().GetRange(etot) - configuration.GetRange().GetRange(e_evt.energy);
    thickness.Fill(thick, ringID);

    if ( (thick > 110) && ( thick < 160) ){
        ede_time.Fill(e_evts[0].energy,
                      double(e_evts[0].timestamp - trigger->timestamp) + (e_evts[0].cfdcorr - trigger->cfdcorr));
        ede_spectra[ringID].Fill(e_evts[0].energy, trigger->energy);
        particle_energy.Fill(etot, ringID);

        auto Ex = configuration.CalculateExcitation(ringID, etot/1e3)*1e3;
        int coinc = 0;
        for ( int i = 0 ; i < buffer.GetDetector(DetectorType::labr).size() ; ++i ){
            auto labr_evt = buffer.GetDetector(DetectorType::labr)[i];
            double time = double(labr_evt->timestamp - trigger->timestamp) + (labr_evt->cfdcorr - trigger->cfdcorr);
            if ( Ex > 7000 )
                ts_ex_above_Sn.Fill(time, labr_evt->detectorID);
            if ( configuration.IsPrompt(time) )
                alfna_prompt.Fill(labr_evt->energy, Ex);
            else if (configuration.IsBackground(time) )
                alfna_background.Fill(labr_evt->energy, Ex);
        }
        mult_ex.Fill(Ex, coinc);
    }



    userSort.FillEvent(buffer);
}

void HistManager::Flush()
{
    for ( auto &type : {DetectorType::labr, DetectorType::deDet, DetectorType::eDet, DetectorType::ppac} ){
        GetSpec(type)->Flush();
    }

    // Next we will get by ring ID
    for ( auto &i : {0, 1, 2, 3, 4, 5, 6, 7}){
        GetPart(i)->Flush();
    }

    userSort.Flush();
}

MTSort::MTSort(TEventQueue_t &input, ThreadSafeHistograms &histograms, const OCL::UserConfiguration &config,
               const char *tree_name, const char *user_sort)
    : input_queue( input )
    , hm( histograms, config, user_sort )
    , tree( ( tree_name ) ? new ROOT::TTreeManager(tree_name) : nullptr )
{
}

void MTSort::Run()
{
    std::pair<std::vector<Entry_t>, size_t> entries;
    while ( !done ){
        if ( input_queue.wait_dequeue_timed(entries, std::chrono::seconds(1)) ){
            if ( entries.second == -1 ){
                Triggered_event event(entries.first);
                hm.AddEntry(event);
            }

            Triggered_event event(entries.first, entries.first[entries.second]);
            hm.AddEntry(event);
            if ( tree ) tree->Fill(event);
        }
    }
    is_done = true;
    Flush();
}

void MTSort::Flush()
{
    hm.Flush();
}

Sorters::Sorters(TEventQueue_t &input, OCL::UserConfiguration &config, const char *tree_name, const char *_user_sort)
    : input_queue( input )
    , histograms( )
    , sorters( )
    , user_config( config )
    , user_sort_path( ( _user_sort ) ? _user_sort : "" )
    , tree_file_name( ( tree_name ) ? tree_name : "" )
    , tree_files( )
{

}

Sorters::~Sorters()
{
    for ( auto &v : sorters ){
        delete v;
    }
}

void Sorters::flush()
{
    for ( auto &v : sorters ){
        v->Flush();
    }
}

MTSort *Sorters::GetNewSorter()
{
    std::string fname = "";
    if ( !tree_file_name.empty() ) {
        fname = tree_file_name.substr(0, tree_file_name.find_last_of('.'));
        fname += "_t" + std::to_string(tree_files.size()) + ".root";
        tree_files.push_back(fname);
    }
    sorters.push_back(new MTSort(input_queue, histograms, user_config,
                                 (fname.empty()) ? nullptr : fname.c_str(),
                                 (user_sort_path.empty()) ? nullptr : user_sort_path.c_str()));
    return sorters.back();
}