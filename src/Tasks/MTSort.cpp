//
// Created by Vetle Wegner Ingeberg on 20/10/2022.
//

#include "MTSort.h"
#include "TTreeManager.h"
#include "ParticleRange.h"

#include <TFile.h>
#include <TTree.h>

using namespace Task;

MT_Detector_Histograms_t::MT_Detector_Histograms_t(ThreadSafeHistograms &hm, const std::string &name, const size_t &num)
    : time( hm.Create2D(std::string("time_"+name), std::string("Time spectra "+name), 30000, -1500, 1500, "Time [ns]", num, 0, num, std::string(name+" ID")) )
    , time_CFDfail( hm.Create2D(std::string("time_"+name+"_CFDfail"), std::string("Time spectra"+name+" CFD fail"), 30000, -1500, 1500, "Time [ns]", num, 0, num, std::string(name+" ID")) )
    , energy( hm.Create2D(std::string("energy_"+name), std::string("Energy spectra "+name), 65536, 0, 65536, "Energy [ch]", num, 0, num, std::string(name+" ID")) )
    , energy_cal( hm.Create2D(std::string("energy_cal_"+name), std::string("energy spectra "+name+" (cal)"), 16384, 0, 16384, "Energy [keV]", num, 0, num, std::string(name+" ID")) )
    , mult( hm.Create1D(std::string("mult_"+name), std::string("Multiplicity " + name), 128, 0, 128, "Multiplicity") )
{}

void MT_Detector_Histograms_t::Fill(const Entry_t &word)
{
    energy.Fill(word.adcvalue, word.detectorID);
    energy_cal.Fill(word.energy, word.detectorID);
}

void MT_Detector_Histograms_t::Fill(const subvector<Entry_t> &subvec,
                                 const Entry_t *start)
{
    mult.Fill(subvec.size());
    for ( auto &entry : subvec ){
        energy.Fill(entry.adcvalue, entry.detectorID);
        energy_cal.Fill(entry.energy, entry.detectorID);
        /*if ( start ){
            if ( start != &entry ){
                time->Fill(double(entry.timestamp - start->timestamp) + (entry.cfdcorr - start->cfdcorr), entry.detectorID);
            }
        }*/
        if ( start && !( (entry.type == start->type)&&(entry.detectorID == start->detectorID)&&(entry.timestamp==start->timestamp)) ){
            if ( entry.cfdfail )
                time_CFDfail.Fill(double(entry.timestamp - start->timestamp) + (entry.cfdcorr - start->cfdcorr), entry.detectorID + 0.5);
            else
                time.Fill(double(entry.timestamp - start->timestamp) + (entry.cfdcorr - start->cfdcorr), entry.detectorID + 0.5);
        }
    }
}

void MT_Detector_Histograms_t::Flush()
{
    time.force_flush();
    energy.force_flush();
    energy_cal.force_flush();
    mult.force_flush();
}

MT_Particle_telescope_t::MT_Particle_telescope_t(ThreadSafeHistograms &hm, const ParticleRange &pr, const size_t &num, const double &_lhs, const double &_rhs)
        : particleRange( pr )
        , lhs( _lhs ), rhs( _rhs )
        , time_e_energy( hm.Create2D("time_e_energy_b"+std::to_string(num), "Time vs E energy",
                                     1000, 0, 20000, "Energy [keV]",
                                     3000, -1500, 1500, "Time, E - dE [ns]"))
        , time_pe_energy(hm.Create2D("time_pe_energy_b"+std::to_string(num), "Particle-gamma time vs E energy",
                                     1000, 0, 20000, "Energy [keV]",
                                     3000, -1500, 1500, "Time, E - labr [ns]"))
        , time_pde_energy(hm.Create2D("time_pde_energy_b"+std::to_string(num), "Particle-gamma time vs dE energy",
                                      1000, 0, 20000, "Energy [keV]",
                                      3000, -1500, 1500, "Time, dE - labr [ns]"))
        , ede_spectra{hm.Create2D("ede_spectra_b"+std::to_string(num)+"f0", "E energy vs dE energy",
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

void MT_Particle_telescope_t::Fill(const subvector<Entry_t> &deltaE, const subvector<Entry_t> &E, const subvector<Entry_t> &labr)
{
    double timediff;
    for ( auto &de : deltaE ){
        for ( auto &e : E ){

            // Check if the same trapzoid. If not, then we will just skip.
            if ( de.detectorID / NUM_SI_DE_TEL != e.detectorID )
                continue;

            timediff = double(e.timestamp - de.timestamp) + (e.cfdcorr - de.cfdcorr);
            //time_de_energy.Fill(de.energy, timediff);
            //time_e_energy.Fill(e.energy, timediff);
            ede_spectra_raw[de.detectorID % NUM_SI_DE_TEL].Fill(e.adcvalue, de.adcvalue);

            // Range
            double range = particleRange(e.energy + de.energy) - particleRange(e.energy);

            if ( range > 110 && range < 150 ) {
                time_e_energy.Fill(e.energy, timediff);
            }

            for ( auto &l : labr ) {
                if (l.cfdfail || l.energy < 2000) // Exclude cfd fails and energy below 2 MeV
                    continue;
                if (de.cfdfail || e.cfdfail)
                    continue;
                double e_timediff = double(e.timestamp - l.timestamp) + e.cfdcorr - l.cfdcorr;
                double de_timediff = double(de.timestamp - l.timestamp) + de.cfdcorr - l.cfdcorr;

                if (range > 110 && range < 150) {
                    time_pe_energy.Fill(e.energy,
                                        e_timediff); // Lets us know what the energy dependence of the E time is
                    time_pde_energy.Fill(de.energy,
                                         de_timediff); // Lets us know what the energy dependence of the dE time is
                }
            }

            // "Simple" timegate. Remove in "production"
            if ( (timediff > lhs && timediff < rhs) && (range > 110 && range < 150) )
                ede_spectra[de.detectorID % NUM_SI_DE_TEL].Fill(e.energy, de.energy);

        }
    }
}

void MT_Particle_telescope_t::Flush()
{
    //time_de_energy.force_flush();
    time_e_energy.force_flush();
    time_pe_energy.force_flush();
    time_pde_energy.force_flush();
    for ( auto &ede : ede_spectra_raw ){
        ede.force_flush();
    }
    for ( auto &ede : ede_spectra )
        ede.force_flush();
}

MT_Ring_t::MT_Ring_t(ThreadSafeHistograms &hm, const ParticleRange &pr, const double &_lhs, const double &_rhs)
    : particleRange( pr )
    , lhs( _lhs ), rhs( _rhs )
    , particle_range( hm.Create2D("particle_range", "Particle range", 2000, 0, 2000, "Range [um]", 8, 0, 8, "Ring ID"))
    , ede_spectra{hm.Create2D("ede_spectra_f0", "Particle identification, ring 0",
                             1000, 0, 20000, "E energy [keV]", 1000, 0, 10000, "dE energy [keV]"),
                 hm.Create2D("ede_spectra_f1", "Particle identification, ring 1",
                             1000, 0, 20000, "E energy [keV]", 1000, 0, 10000, "dE energy [keV]"),
                 hm.Create2D("ede_spectra_f2", "Particle identification, ring 2",
                             1000, 0, 20000, "E energy [keV]", 1000, 0, 10000, "dE energy [keV]"),
                 hm.Create2D("ede_spectra_f3", "Particle identification, ring 3",
                             1000, 0, 20000, "E energy [keV]", 1000, 0, 10000, "dE energy [keV]"),
                 hm.Create2D("ede_spectra_f4", "Particle identification, ring 4",
                             1000, 0, 20000, "E energy [keV]", 1000, 0, 10000, "dE energy [keV]"),
                 hm.Create2D("ede_spectra_f5", "Particle identification, ring 5",
                             1000, 0, 20000, "E energy [keV]", 1000, 0, 10000, "dE energy [keV]"),
                 hm.Create2D("ede_spectra_f6", "Particle identification, ring 6",
                             1000, 0, 20000, "E energy [keV]", 1000, 0, 10000, "dE energy [keV]"),
                 hm.Create2D("ede_spectra_f7", "Particle identification, ring 7",
                             1000, 0, 20000, "E energy [keV]", 1000, 0, 10000, "dE energy [keV]"),
                }
{}


void MT_Ring_t::Fill(const subvector<Entry_t> &deltaE, const subvector<Entry_t> &E)
{
    double timediff = 0;

    for ( auto &e : E ){
        for ( auto &de : deltaE ){

            // Skip if de is not in the same telescope
            if ( de.detectorID / NUM_SI_DE_TEL != e.detectorID )
                continue;

            timediff = double(de.timestamp - e.timestamp);
            timediff += de.cfdcorr - e.cfdcorr;

            if ( timediff > lhs && timediff < rhs ) {
                ede_spectra[de.detectorID % NUM_SI_DE_TEL].Fill(e.energy, de.energy);
                double thick = particleRange(e.energy + de.energy) - particleRange(e.energy);
                particle_range.Fill(thick, de.detectorID % NUM_SI_DE_TEL);
            }
        }
    }
}

void MT_Ring_t::Flush()
{
    //particle_range.force_flush();
    for ( auto &ede : ede_spectra ){
        ede.force_flush();
    }
}

MTHistManager::MTHistManager(ThreadSafeHistograms &histograms, const ParticleRange &particleRange, const char *custom_sort)
        : labr( histograms, "labr", NUM_LABR_DETECTORS )
        , si_de( histograms, "si_de", NUM_SI_DE_DET )
        , si_e( histograms, "si_e", NUM_SI_E_DET )
        , ppacs( histograms, "ppac", NUM_PPAC )
        , ring_analysis(histograms, particleRange, -100, 50)
        , particle_coincidence{{ histograms, particleRange, 0, -100, 50},
                               { histograms, particleRange, 1, -100, 50},
                               { histograms, particleRange, 2, -100, 50},
                               { histograms, particleRange, 3, -100, 50},
                               { histograms, particleRange, 4, -100, 50},
                               { histograms, particleRange, 5, -100, 50},
                               { histograms, particleRange, 6, -100, 50},
                               { histograms, particleRange, 7, -100, 50}}
        , userSort( histograms, custom_sort )
{
}

MT_Detector_Histograms_t *MTHistManager::GetSpec(const DetectorType &type)
{
    switch ( type ) {
        case DetectorType::labr : return &labr;
        case DetectorType::deDet : return &si_de;
        case DetectorType::eDet : return &si_e;
        case DetectorType::ppac : return &ppacs;
        default : return nullptr;
    }
}

void MTHistManager::AddEntry(Triggered_event &buffer)
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
        GetPart(trap)->Fill(de_evts, e_evts, buffer.GetDetector(DetectorType::labr));
        ring_analysis.Fill(de_evts, e_evts);
    }

    userSort.FillEvent(buffer);
}

void MTHistManager::Flush()
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

/*MTSort::MTSort(Task::TEventQueue_t &input,ThreadSafeHistograms &histograms,
               const ParticleRange &particleRange, const char *tree_name, const char *custom_sort)
    : input_queue( input )
    , hm( histograms, particleRange, custom_sort )
    , tree( ( tree_name ) ? new ROOT::TTreeManager(tree_name) : nullptr )
{
}*/

MTSort::MTSort(Task::TEventQueue_t &input,ThreadSafeHistograms &histograms,
               const ParticleRange &particleRange, std::shared_ptr<TFile> file, const char *custom_sort)
        : input_queue( input )
        , hm( histograms, particleRange, custom_sort )
        , tree( ( file ) ? new ROOT::TTreeManager(file) : nullptr )
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

Sorters::Sorters(TEventQueue_t &input, const ParticleRange &pr, const char *tree_name, const char *_user_sort)
    : input_queue( input )
    , histograms( )
    , particleRange( pr )
    , sorters( )
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


    sorters.push_back(new MTSort(input_queue, histograms, particleRange,
                                 ( fname.empty() ) ? nullptr : std::make_shared<TFile>(fname.c_str(), "RECREATE"),
                                 (user_sort_path.empty()) ? nullptr : user_sort_path.c_str()));
    return sorters.back();
}

MTSort *Sorters::GetNewSorter(std::shared_ptr<TFile> file)
{
    sorters.push_back(new MTSort(input_queue, histograms, particleRange,
                                 (file) ?  std::move(file) : nullptr,
                                 (user_sort_path.empty()) ? nullptr : user_sort_path.c_str()));
    return sorters.back();
}