//
// Created by Vetle Wegner Ingeberg on 04/04/2023.
//

#include <UserSort/UserSort.h>
#include <histogram/ThreadSafeHistograms.h>
#include <Format/event.h>

#include <PhysicalParam/ParticleRange.h>

const double a0[] = {14.008496, 14.020293, 14.032383, 14.044716,
                     14.057543, 14.070141, 14.082770, 14.095125};
const double a1[] = {-1.214118, -1.209304, -1.204287, -1.199069,
                     -1.193729,-1.188110, -1.182296, -1.176236};
const double a2[] = {0.002505, 0.002386, 0.002263, 0.002135,
                     0.002006,0.001869, 0.001726, 0.001577};

inline constexpr double CalcEx(const double &E, const int &ringID)
{
    double e = E/1e3;
    return (a0[ringID] + a1[ringID]*e + a2[ringID]*e*e)*1e3;
}

class TimingInvestigation : public UserSort
{
private:
    ThreadSafeHistogram2D de_time_energy[NUM_SI_DE_TEL];
    ThreadSafeHistogram2D e_time_energy[NUM_SI_E_DET];
    ThreadSafeHistogram2D ede_energy, ex_energy, time_de_hoyle, ex_eg_matrix;
    ThreadSafeHistogram2D gamgam_labr, time_energy_labr_hoyle;
    ThreadSafeHistogram2D gamgam_time, gamgam_satelite, gamgam_prompt, gamgam_prompt_satelite;
    ThreadSafeHistogram1D range;
    ThreadSafeHistogram2D thick_mult, thick_time, mult_two_ring_ids;
    ThreadSafeHistogram1D de_mult, de_time_mult;
    ParticleRange pr;

public:
    TimingInvestigation(ThreadSafeHistograms *hist);
    void FillEvent(const Triggered_event &event) override;
    void Flush() override;

};

extern "C" {
    UserSort *NewUserSort(ThreadSafeHistograms *hist){
        return new TimingInvestigation(hist);
    }
}

TimingInvestigation::TimingInvestigation(ThreadSafeHistograms *hist)
    : de_time_energy{ hist->Create2D("de_time_energy_f0", "de time vs energy f0", 1000, 0, 8000, "Energy [keV]", 1000, -500, 500, "dE - E time [ns]"),
                      hist->Create2D("de_time_energy_f1", "de time vs energy f1", 1000, 0, 8000, "Energy [keV]", 1000, -500, 500, "dE - E time [ns]"),
                      hist->Create2D("de_time_energy_f2", "de time vs energy f2", 1000, 0, 8000, "Energy [keV]", 1000, -500, 500, "dE - E time [ns]"),
                      hist->Create2D("de_time_energy_f3", "de time vs energy f3", 1000, 0, 8000, "Energy [keV]", 1000, -500, 500, "dE - E time [ns]"),
                      hist->Create2D("de_time_energy_f4", "de time vs energy f4", 1000, 0, 8000, "Energy [keV]", 1000, -500, 500, "dE - E time [ns]"),
                      hist->Create2D("de_time_energy_f5", "de time vs energy f5", 1000, 0, 8000, "Energy [keV]", 1000, -500, 500, "dE - E time [ns]"),
                      hist->Create2D("de_time_energy_f6", "de time vs energy f6", 1000, 0, 8000, "Energy [keV]", 1000, -500, 500, "dE - E time [ns]"),
                      hist->Create2D("de_time_energy_f7", "de time vs energy f7", 1000, 0, 8000, "Energy [keV]", 1000, -500, 500, "dE - E time [ns]")
                    }
    , e_time_energy{hist->Create2D("e_time_energy_b0", "e time vs energy f0", 1000, 0, 20000, "Energy [keV]", 1000, -500, 500, "E - dE time [ns]"),
                    hist->Create2D("e_time_energy_b1", "e time vs energy f1", 1000, 0, 20000, "Energy [keV]", 1000, -500, 500, "E - dE time [ns]"),
                    hist->Create2D("e_time_energy_b2", "e time vs energy f2", 1000, 0, 20000, "Energy [keV]", 1000, -500, 500, "E - dE time [ns]"),
                    hist->Create2D("e_time_energy_b3", "e time vs energy f3", 1000, 0, 20000, "Energy [keV]", 1000, -500, 500, "E - dE time [ns]"),
                    hist->Create2D("e_time_energy_b4", "e time vs energy f4", 1000, 0, 20000, "Energy [keV]", 1000, -500, 500, "E - dE time [ns]"),
                    hist->Create2D("e_time_energy_b5", "e time vs energy f5", 1000, 0, 20000, "Energy [keV]", 1000, -500, 500, "E - dE time [ns]"),
                    hist->Create2D("e_time_energy_b6", "e time vs energy f6", 1000, 0, 20000, "Energy [keV]", 1000, -500, 500, "E - dE time [ns]"),
                    hist->Create2D("e_time_energy_b7", "e time vs energy f7", 1000, 0, 20000, "Energy [keV]", 1000, -500, 500, "E - dE time [ns]")
               }
    , ex_energy( hist->Create2D("ex_energy", "Excitation energy", 5000, 0, 15000, "Energy [keV]", 8, 0, 8, "Ring ID") )
    , time_de_hoyle( hist->Create2D("time_de_hoyle", "Time dE vs Hoyle proton", 1000, -500, 500, "Timediff dE - dE(Hoyle) [ns]", NUM_SI_DE_DET, 0, NUM_SI_DE_DET, "dE detector ID") )
    , ex_eg_matrix( hist->Create2D("excitation_gamma", "Excitation vs. gamma-ray energy", 1000, 0, 10000, "LaBr3:Ce energy [keV]", 1200, -2000, 10000, "Excitation energy [keV]") )
    , ede_energy( hist->Create2D("ede_energy", "e + de energy", 5000, 0, 20000, "Particle energy [keV]", 8, 0, 8, "ring ID") )
    , gamgam_labr( hist->Create2D("gamgam_labr", "labr vs de time", 10000, -1000, 1000, "Timediff [ns]",
                                  30, 0, 30, "LaBr ID") )
    , time_energy_labr_hoyle( hist->Create2D("time_energy_labr_hoyle", "time erngy", 1000, 0, 5000, "LaBr energy [keV]", 1000, -250, 250, "Time labr - de [ns]") )
    , gamgam_time( hist->Create2D("gamgam_time", "Gamma gamma matrix 16 vs 26",
                                  5000, -250, 250, "Timediff [ns]",
                                  3, 0, 3, "WhatIs") )
    , gamgam_satelite( hist->Create2D("gamgam_satelite", "Gamma gamma matrix 16 vs 26",
                                      5000, -250, 250, "Timediff [ns]",
                                      3, 0, 3, "WhatIs") )
    , gamgam_prompt( hist->Create2D("gamgam_prompt", "Gamma gamma matrix 16 vs 26",
                                    5000, -250, 250, "Timediff [ns]",
                                    3, 0, 3, "WhatIs") )
    , gamgam_prompt_satelite( hist->Create2D("gamgam_prompt_satelite", "Gamma gamma matrix 16 vs 26",
                                    5000, -250, 250, "Timediff [ns]",
                                    3, 0, 3, "WhatIs") )
    , range( hist->Create1D("range", "Thickness of dE", 1000, 0, 1000, "Thickness [um]") )
    , thick_mult( hist->Create2D("thickness", "Thickness with mult == 2", 1000, 0, 1000, "Thickness [um]",
                                 3, 0, 3, "dE event (last bin = sum)") )
    , thick_time( hist->Create2D("thickness_time", "Thickness with mult == 2 vs time", 1000, 0, 1000, "Thickness [um]",
                                 1000, 0, 1000, "Time between dE events [ns]") )
    , mult_two_ring_ids( hist->Create2D("mult_two_ringID", "Ring vs Ring", 8, 0, 8, "Ring Number (lowest)", 8, 0, 8, "Ring Number (highest)") )
    , de_mult( hist->Create1D("de_mult", "dE multiplicity within ± 100 ns of a E event",
                              10, 0, 10, "Number of dE events in same trapezoid") )
    , de_time_mult( hist->Create1D("de_time_mult", "Time difference between dE in dE mult = 2 events",
                                   5000, 0, 500, "Timediff [ns]") )
    , pr( "/Users/vetlewi/Git_Repositories/vetlewi/XIA2tree/cal/zrange_p.dat" )
{}

struct particle_events {
    Entry_t e_event;
    std::vector<Entry_t> de_events;
};

struct time_data {
    long long timestamp;
    double time_correction;
};

inline double operator-(const time_data &lhs, const time_data &rhs){
    return double(lhs.timestamp - rhs.timestamp) + (lhs.time_correction - rhs.time_correction);
}

struct particle_event {
    int telescopeID;
    double e_energy;
    time_data e_time;

    int ringID;
    double de_energy;
    time_data de_time;
};

void TimingInvestigation::FillEvent(const Triggered_event &event)
{

    // First we will determine if we have one and only one particle. That means that we require only 1 particle in the
    // dE detector and one particle in the E detector
    auto deEvts = event.GetDetector(DetectorType::deDet);
    auto eEvts = event.GetDetector(DetectorType::eDet);

    if ( deEvts.size() ==1 ){
        auto deEvt = deEvts[0];
        if ( deEvt->energy > 1150 && deEvt->energy < 1455 && !deEvt->cfdfail && deEvt->detectorID % 8 < 7 ){

            for ( auto &l : event.GetDetector(DetectorType::labr) ){
                if ( l.energy < 2000 )
                    continue;
                double timediff = double(l.timestamp - deEvt->timestamp) + l.cfdcorr - deEvt->cfdcorr;

                gamgam_labr.Fill(timediff, l.detectorID);

            }

        }
    }

    return;
    std::vector<particle_events> pevents;
    std::vector<particle_event> true_particle_events;
    for ( auto &e : eEvts ){
        particle_events pevt;
        pevt.e_event = e;
        for ( auto &de : deEvts){
            if ( e.cfdfail || de.cfdfail || de.detectorID / NUM_SI_DE_TEL != e.detectorID )
                continue;
            double timediff = double(e.timestamp - de.timestamp) + e.cfdcorr - de.cfdcorr;
            double thick = pr( de.energy + e.energy ) - pr( e.energy );
            //if ( thick < 110 || thick > 150 )
            //    continue;
            e_time_energy[e.detectorID].Fill(e.energy, timediff);
            if ( timediff > -41 && timediff < 100 ){
                pevt.de_events.push_back(de);
            }
        }
        if ( !pevt.de_events.empty() )
            pevents.push_back(pevt);

    }

    if ( !pevents.empty() ){
        for ( auto &pevt : pevents ){
            de_mult.Fill(pevt.de_events.size());
            if ( pevt.de_events.size() == 1 ){
                true_particle_events.push_back({
                                                       int(pevt.e_event.detectorID),
                                                       pevt.e_event.energy,
                                                       {pevt.e_event.timestamp, pevt.e_event.cfdcorr},
                                                       int(pevt.de_events[0].detectorID % NUM_SI_DE_TEL),
                                                       pevt.de_events[0].energy,
                                                       {pevt.de_events[0].timestamp, pevt.de_events[0].cfdcorr}
                                               });
            } else if ( pevt.de_events.size() == 2 ){
                auto rID0 = pevt.de_events[0].detectorID % NUM_SI_DE_TEL;
                auto rID1 = pevt.de_events[1].detectorID % NUM_SI_DE_TEL;
                auto rmax = ( rID0 > rID1 ) ? rID0 : rID1;
                auto rmin = ( rID0 <= rID1 ) ? rID0 : rID1;
                mult_two_ring_ids.Fill(rmin+0.5, rmax+0.5);

                // Filter out not next to each other
                if ( rmax - rmin != 1 )
                    continue;

                auto timediff = double(pevt.de_events[1].timestamp - pevt.de_events[0].timestamp);
                timediff += (pevt.de_events[1].cfdcorr - pevt.de_events[0].cfdcorr);
                de_time_mult.Fill(std::abs(timediff));
                double r_back = pr(pevt.e_event.energy);
                double thick = pr( pevt.de_events[0].energy + pevt.e_event.energy ) - r_back;
                thick_mult.Fill(thick, 0.5);
                thick = pr( pevt.de_events[1].energy + pevt.e_event.energy ) - r_back;
                thick_mult.Fill(thick, 1.5);
                thick = pr(pevt.de_events[0].energy + pevt.de_events[1].energy+ pevt.e_event.energy ) - r_back;
                thick_mult.Fill(thick, 2.5);
                thick_time.Fill(thick, timediff);
            }
        }
    }

    for ( auto &particle_event : true_particle_events ){
        double ex = CalcEx(particle_event.e_energy+particle_event.de_energy, particle_event.ringID);
        for ( auto &l : event.GetDetector(DetectorType::labr) ){
            double timediff = time_data{l.timestamp, l.cfdcorr} - particle_event.de_time;


            if ( particle_event.ringID < 5 ) {
                gamgam_labr.Fill(timediff, l.detectorID);
                if ( l.detectorID == 16 && ( ex > 7500 && ex < 7900 ) )
                    time_energy_labr_hoyle.Fill(l.energy, timediff);
            }

            if ( (timediff < 3 && timediff > -3) && particle_event.ringID < 5 ){
                ex_eg_matrix.Fill(l.energy, ex);
            }

            ex_energy.Fill(ex, particle_event.ringID);
            ede_energy.Fill(particle_event.de_energy+particle_event.e_energy, particle_event.ringID);
        }
        if ( particle_event.ringID < 5 && ( ex > 7500 && ex < 7900 ) ) {
            for (auto &de: deEvts) {
                if (de.detectorID / NUM_SI_DE_TEL == particle_event.telescopeID)
                    continue;

                // Check that there is no E detectors in coincidence
                if ( std::any_of(eEvts.begin(), eEvts.end(), [de](const Entry_t &entry){ return (entry.detectorID == de.detectorID / NUM_SI_DE_TEL); }) )
                    continue;


                // Fill a coincidence spectra...
                double timediff = time_data{de.timestamp, de.cfdcorr} - particle_event.de_time;
                time_de_hoyle.Fill(timediff, de.detectorID);

            }
        }

    }

    // Now we will return... I will come back later to stuff below...
    //return;

    // Check that there is exactly 1 particle in both E and dE counter
    if ( deEvts.size() != 1 || eEvts.size() != 1 )
        return;

    auto deEvt = *deEvts[0];
    auto eEvt = *eEvts[0];

    // Require that the hits are in the same trapezoid
    if ( deEvt.detectorID / NUM_SI_DE_TEL != eEvt.detectorID )
        return;

    double thick = pr(deEvt.energy + eEvt.energy) - pr(eEvt.energy);
    range.Fill(thick);

    if ( thick < 115 || thick > 145 )
        return;

    // Next we will fill the particle time spectra
    if ( !eEvt.cfdfail && !deEvt.cfdfail ) {
        auto timediff = double(deEvt.timestamp - eEvt.timestamp);
        timediff += deEvt.cfdcorr - eEvt.cfdcorr;
        de_time_energy[deEvt.detectorID % NUM_SI_DE_TEL].Fill(deEvt.energy, timediff);
        //e_time_energy[eEvt.detectorID].Fill(eEvt.energy, timediff);
        if (timediff > 110 || timediff < -270)
            return;
    }

    // Find all the labr with correct ID's
    auto labr16 = std::vector<Entry_t>();
    auto labr26 = std::vector<Entry_t>();

    for ( auto &evt : event.GetDetector(DetectorType::labr) ){
        if ( evt.detectorID == 16 )
            labr16.push_back(evt);
        if ( evt.detectorID == 26 )
            labr26.push_back(evt);
    }

    auto *trigger = event.GetTrigger();

    // Loop over our events...
    for ( auto &labr16evt : labr16 ){
        for ( auto &labr26evt : labr26 ){
            auto timediff_gamma = double(labr26evt.timestamp - labr16evt.timestamp);
            timediff_gamma += labr26evt.cfdcorr - labr16evt.cfdcorr;

            if ( !labr16evt.cfdfail && !labr26evt.cfdfail ) {
                gamgam_time.Fill(timediff_gamma, 2.5);
                if ( !deEvt.cfdfail ){
                    auto timediff16 = double(labr16evt.timestamp - deEvt.timestamp);
                    timediff16 += labr16evt.cfdcorr - deEvt.cfdcorr;
                    gamgam_time.Fill(timediff16, 0.5);
                    auto timediff26 = double(labr26evt.timestamp - deEvt.timestamp);
                    timediff26 += labr26evt.cfdcorr - deEvt.cfdcorr;
                    gamgam_time.Fill(timediff26, 1.5);
                    if ( (timediff16 < -10) && (timediff16 > -20) ){
                        gamgam_satelite.Fill(timediff16, 0.5);
                        gamgam_satelite.Fill(timediff26, 1.5);
                        gamgam_satelite.Fill(timediff_gamma, 2.5);
                    }
                    if ( timediff_gamma < 3 && timediff_gamma > -3 ){
                        gamgam_prompt.Fill(timediff16, 0.5);
                        gamgam_prompt.Fill(timediff26, 1.5);
                        gamgam_prompt.Fill(timediff_gamma, 2.5);
                    }
                    if ( (timediff16 > -3 && timediff16 < 3) ){
                        gamgam_prompt_satelite.Fill(timediff16, 0.5);
                        gamgam_prompt_satelite.Fill(timediff26, 1.5);
                        gamgam_prompt_satelite.Fill(timediff_gamma, 2.5);
                    }
                }
            }


            /*if ( trigger ) {
                auto evt16time = double(labr16evt.timestamp - trigger->timestamp);
                evt16time += labr16evt.cfdcorr - trigger->cfdcorr;
                auto evt26time = double(labr26evt.timestamp - trigger->timestamp);
                evt26time += labr26evt.cfdcorr - trigger->cfdcorr;
                gamgam_time.Fill(evt16time, 0.5);
                gamgam_time.Fill(evt26time, 1.5);
            }*/
        }
    }
}

void TimingInvestigation::Flush()
{
    gamgam_time.force_flush();
    for ( auto &m : de_time_energy )
        m.force_flush();
    for ( auto &m : e_time_energy )
        m.force_flush();
    range.force_flush();
    gamgam_satelite.force_flush();
    gamgam_prompt.force_flush();
    gamgam_prompt_satelite.force_flush();
}