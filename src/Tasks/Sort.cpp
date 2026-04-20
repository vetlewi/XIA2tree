//
// Created by Vetle Wegner Ingeberg on 06/03/2026.
//

#include "Sort.h"

#include "TTreeManager.h"
#include "ParticleRange.h"

using namespace Task;

Detector_Histograms_t::Detector_Histograms_t(Histograms &hm, const std::string &name, const size_t &num)
    : time( hm.Create2D(std::string("time_"+name), std::string("Time spectra "+name), 30000, -1500, 1500, "Time [ns]", num, 0, num, std::string(name+" ID")) )
    , time_CFDfail( hm.Create2D(std::string("time_"+name+"_CFDfail"), std::string("Time spectra"+name+" CFD fail"), 30000, -1500, 1500, "Time [ns]", num, 0, num, std::string(name+" ID")) )
    , energy( hm.Create2D(std::string("energy_"+name), std::string("Energy spectra "+name), 65536, 0, 65536, "Energy [ch]", num, 0, num, std::string(name+" ID")) )
    , energy_cal( hm.Create2D(std::string("energy_cal_"+name), std::string("energy spectra "+name+" (cal)"), 16384, 0, 16384, "Energy [keV]", num, 0, num, std::string(name+" ID")) )
    , mult( hm.Create1D(std::string("mult_"+name), std::string("Multiplicity " + name), 128, 0, 128, "Multiplicity") )
{}

void Detector_Histograms_t::Fill(const Entry_t &word)
{
    energy->Fill(word.adcvalue, word.detectorID);
    energy_cal->Fill(word.energy, word.detectorID);
}

void Detector_Histograms_t::Fill(const subvector<Entry_t> &subvec,
                                 const Entry_t *start)
{
    mult->Fill(subvec.size());
    for ( auto &entry : subvec ){
        energy->Fill(entry.adcvalue, entry.detectorID);
        energy_cal->Fill(entry.energy, entry.detectorID);

        if ( start && !( (entry.type == start->type)&&(entry.detectorID == start->detectorID)&&(entry.timestamp==start->timestamp)) ){
            if ( entry.cfdfail ) {
                time_CFDfail->Fill(double(entry.timestamp - start->timestamp) + (entry.cfdcorr - start->cfdcorr), entry.detectorID + 0.5);
            } else {
                time->Fill(double(entry.timestamp - start->timestamp) + (entry.cfdcorr - start->cfdcorr), entry.detectorID + 0.5);
            }
        }
    }
}

Particle_telescope_t::Particle_telescope_t(Histograms &hm, const size_t &num)
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
{
    for ( const auto &de : deltaE ){
        for ( const auto &e : E ){

            // Check if the same trapzoid. If not, then we will just skip.
            if ( de.detectorID / NUM_SI_DE_TEL != e.detectorID )
                continue;

            ede_spectra[de.detectorID % NUM_SI_DE_TEL]->Fill(e.energy, de.energy);
            ede_spectra_raw[de.detectorID % NUM_SI_DE_TEL]->Fill(e.adcvalue, de.adcvalue);
        }
    }
}

HistManager::HistManager(Histograms &histograms, const OCL::UserConfiguration &user_config, const char *custom_sort)
        : configuration( user_config )
        , labr( histograms, "labr", user_config.GetNumDetectors(DetectorType::labr) )
        , si_de( histograms, "si_de", user_config.GetNumDetectors(DetectorType::deDet) )
        , si_e( histograms, "si_e", user_config.GetNumDetectors(DetectorType::eDet) )
        , ppacs( histograms, "ppac", user_config.GetNumDetectors(DetectorType::ppac) )
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
        , labr_energy_gated( histograms.Create2D("labr_energy_gated", "Uncalibrated LaBr3 - particle gated",
                                            65536, 0, 65536, "Energy [ch]",
                                                 NUM_LABR_DETECTORS, 0, NUM_LABR_DETECTORS, "Detector ID") )
        , labr_energy_cal_gated( histograms.Create2D("labr_energy_cal_gated", "Calibrated LaBr3 - particle gated",
                                            32768, 0, 32768, "Energy [keV]",
                                            NUM_LABR_DETECTORS, 0, NUM_LABR_DETECTORS, "Detector ID") )
        , alfna_prompt( histograms.Create2D("alfna_prompt", "Particle - gamma coincidence",
                                            1500, 0, 15000, "E gamma [keV]",
                                            1700, -2000, 15000, "Excitation [keV]") )
        , alfna_background( histograms.Create2D("alfna_background", "Particle - gamma coincidence",
                                                1500, 0, 15000, "E gamma [keV]",
                                                1700, -2000, 15000, "Excitation [keV]") )
        , ts_ex_above_Sn( histograms.Create2D("ts_ex_above_Sn", "Time spectra (LaBr) above Sn",
                                             30000, -1500, 1500, "Time [ns]",
                                             NUM_LABR_DETECTORS, 0, NUM_LABR_DETECTORS, "Detector ID") )
        , mult_ex( histograms.Create2D("mult_ex", "Multiplicity as function of Ex (± 10 ns)",
                                                 15000, 0, 15000, "Excitation energy [keV]",
                                                    20, 0, 20, "Multiplicity") )
        , chargeIntegrator( histograms.Create1D("chargeIntegrator", "Charge integrator", 86400, 0, 86400, "Time [ns]") )
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
    userSort.FillEvent(buffer);
    const auto *trigger = buffer.GetTrigger();

    // We get the qint and increment the time spectrum.
    for ( const auto& Qint : buffer.GetDetector(DetectorType::qint) ) {
        auto time = double(Qint.timestamp) / 1e9; // Convert to second
        chargeIntegrator->Fill(time);
    }

    // For now, we will discard events with bad CFD
    // We have this req. if we get a trigger
    if ( trigger )
        if ( trigger->cfdfail )
            return;

    for ( auto &type : {DetectorType::labr, DetectorType::deDet, DetectorType::eDet, DetectorType::ppac} ){
        GetSpec(type)->Fill(buffer.GetDetector(type), trigger);
    }

    // No trigger, turn over to the userSort
    if ( !trigger ) {
        return;
    }

    if ( trigger->type != DetectorType::deDet ) { // All analysis below requires the trigger to be a dE detector
        return;
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
    thickness->Fill(thick, ringID);

    if ( configuration.GetAnalysisParameters().ParticleGatePass(thick) ){
        ede_time->Fill(e_evts[0].energy,
                      double(e_evts[0].timestamp - trigger->timestamp) + (e_evts[0].cfdcorr - trigger->cfdcorr));
        ede_spectra[ringID]->Fill(e_evts[0].energy, trigger->energy);
        particle_energy->Fill(etot, ringID);

        auto Ex = configuration.GetAnalysisParameters().CalculateExcitation(ringID, etot/1e3)*1e3;
        int coinc = 0;
        for ( int i = 0 ; i < buffer.GetDetector(DetectorType::labr).size() ; ++i ){
            auto labr_evt = buffer.GetDetector(DetectorType::labr)[i];
            double time = double(labr_evt->timestamp - trigger->timestamp) + (labr_evt->cfdcorr - trigger->cfdcorr);
            if ( Ex > 7000 )
                ts_ex_above_Sn->Fill(time, labr_evt->detectorID);
            if ( configuration.GetAnalysisParameters().IsPrompt(time) ) {
                alfna_prompt->Fill(labr_evt->energy, Ex);
                labr_energy_gated->Fill(labr_evt->adcvalue, labr_evt->detectorID);
                labr_energy_cal_gated->Fill(labr_evt->energy, labr_evt->detectorID);
            } else if (configuration.GetAnalysisParameters().IsBackground(time) )
                alfna_background->Fill(labr_evt->energy, Ex);
        }
        mult_ex->Fill(Ex, coinc);
    }
}

Sorter::Sorter(TEventQueue_t &input, const OCL::UserConfiguration &config,
               const char *tree_name, const bool& keep_traces, const char *user_sort)
    : input_queue( input )
    , histograms( )
    , hm( histograms, config, user_sort )
    , userConfig( config )
    , tree( ( tree_name ) ? new ROOT::TTreeManager(tree_name, keep_traces) : nullptr )
{
}

void Sorter::Run() {
    std::pair<std::vector<Entry_t>, int> entries;
    while ( input_queue.is_not_finish() || !input_queue.empty() ) {
        if ( !input_queue.try_pop(entries) ) {
            std::this_thread::yield();
            continue;
        }
        if ( entries.first.empty() )
            continue;
        if ( userConfig.GetSortType() == CLI::sort_type::gap ){
            if ( entries.second > 0 )
                continue;
            Triggered_event event(entries.first);
            hm.AddEntry(event);
            if ( tree ) tree->Fill(event);
        } else {
            Triggered_event event(entries.first, entries.first[entries.second]);
            hm.AddEntry(event);
            if ( tree ) tree->Fill(event);
        }
    }
    is_done = true;
    tree.reset(nullptr);
}