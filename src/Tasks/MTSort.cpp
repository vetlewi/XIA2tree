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
                                  2048, 0, 15000, "E energy [keV]", 2048, 0, 16384, "dE energy [keV]")}
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

void Particle_telescope_t::Fill(const subvector<Entry_t> &deltaE, const subvector<Entry_t> &E)
{
    for ( auto &de : deltaE ){
        for ( auto &e : E ){

            // Check if the same trapzoid. If not, then we will just skip.
            if ( de.detectorID / NUM_SI_DE_TEL != e.detectorID )
                continue;

            ede_spectra[de.detectorID % NUM_SI_DE_TEL].Fill(e.adcvalue, de.adcvalue);
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



    // Next we will get by back ID
    for ( auto &trap : {0, 1, 2, 3, 4, 5, 6, 7}){
        auto [de_evts, e_evts] = buffer.GetTrap(trap);
        GetPart(trap)->Fill(de_evts, e_evts);
        for ( auto &e_entry : e_evts ){
            ede_time.Fill(e_entry.energy,
                          double(e_entry.timestamp - trigger->timestamp) + (e_entry.cfdcorr - trigger->cfdcorr));
        }
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