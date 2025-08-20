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
#include <algorithm>
#include <numeric>

double a0[] = {15.578528, 15.579984, 15.581394, 15.582739, 15.583966, 15.585056, 15.585966, 15.586646};
double a1[] = {-1.035590, -1.034681, -1.033726, -1.032726, -1.031676, -1.030575, -1.029422, -1.028214};
double a2[] = {0.000272, 0.000252, 0.000232, 0.000211, 0.000188, 0.000164, 0.000139, 0.000112};

constexpr double CalculateEx(const double &Etot, const int &ringID){
    return a0[ringID] + a1[ringID] * Etot + a2[ringID] * Etot*Etot;
}

class ParticleCoincidence : public UserSort
{
private:
    const OCL::UserConfiguration *user_config;
    double thickness;
    long long counts;

    ThreadSafeHistogram2D de_thickness;
    ThreadSafeHistogram2D particle_energy;
    ThreadSafeHistogram2D excitation_energy;
    ThreadSafeHistogram2D exgam, exgam_bg, exgam_full;
    ThreadSafeHistogram2D gamgam;
    ThreadSafeHistogram1D part_gamgam, sum_ex_gam;
    ThreadSafeHistogram1D gam_part_time;

public:
    ParticleCoincidence(ThreadSafeHistograms *hist, const OCL::UserConfiguration *user_config);
    ~ParticleCoincidence() override = default;
    void FillEvent(const Triggered_event &event) override;
    void Flush() override;

};

extern "C" {
    UserSort *NewUserSort(ThreadSafeHistograms *hist, const OCL::UserConfiguration *config){
        return new ParticleCoincidence(hist, config);
    }
}

ParticleCoincidence::ParticleCoincidence(ThreadSafeHistograms *hist, const OCL::UserConfiguration *config)
    : user_config( config )
    , thickness( 0 )
    , counts( 0 )
    , de_thickness( hist->Create2D("de_thickness", "dE thickness", 1000, 0, 1000, "Thickness [um]", 8, 0, 8, "Ring #") )
    , particle_energy( hist->Create2D("particle_energy", "Total particle energy", 16384, 0, 16384, "Particle energy [keV]", 8, 8, 8, "Ring #") )
    , excitation_energy( hist->Create2D("excitation", "Ex energy", 10000, 0, 10000, "Excitation energy [keV]", 8, 0, 8, "Ring #") )
    , exgam( hist->Create2D("exgam", "Excitation gamma spectrum", 5000, 0, 10000, "LaBr3:Ce energy [keV]", 7000, -2000, 12000, "Excitation Energy [keV]") )
    , exgam_bg( hist->Create2D("exgam_bg", "Excitation gamma spectrum", 5000, 0, 10000, "LaBr3:Ce energy [keV]", 7000, -2000, 12000, "Excitation Energy [keV]") )
    , exgam_full( hist->Create2D("exgam_full", "Excitation gamma spectrum", 5000, 0, 10000, "LaBr3:Ce energy [keV]", 7000, -2000, 12000, "Excitation Energy [keV]") )
    , gamgam( hist->Create2D("gamgam", "Gamma vs Gamma", 1000, 0, 10000, "Gamma 1 [keV]", 1000, 0, 10000, "Gamma 2 [keV]") )
    , part_gamgam( hist->Create1D("part_gamgam", "Particle, gamma gamma", 1000, 0, 5000, "Energy [keV]") )
    , sum_ex_gam( hist->Create1D("sum_ex_gam", "Excitation EgamTot difference", 1000, -5000, 5000, "Energy difference [keV]") )
    , gam_part_time( hist->Create1D("gam_part_time", "Gamma particle time", 1000, -500, 500, "Time [ns]") )
{

}

void ParticleCoincidence::FillEvent(const Triggered_event &event)
{
    auto trigger = event.GetTrigger();
    auto trapID = trigger->detectorID / 8;
    auto ringID = trigger->detectorID % 8;
    auto [de_evts, e_evts] = event.GetTrap(trapID);

    if ( e_evts.size() != 1 || de_evts.size() != 1 )
        return;

    auto e_evt = e_evts[0];
    double etot = e_evt.energy + trigger->energy;

    double thick = user_config->GetRange().GetRange(etot) - user_config->GetRange().GetRange(e_evt.energy);
    de_thickness.Fill(thick, ringID);

    double excitation = CalculateEx(etot*1e-3, ringID) * 1e3;
    excitation_energy.Fill(excitation, ringID);

    std::vector<Entry_t> coincident_gammas;
    for ( const auto& labr_event : event.GetDetector(DetectorType::labr) ){
        double tdiff = double(labr_event.timestamp - trigger->timestamp) + (labr_event.cfdcorr - trigger->cfdcorr);
        gam_part_time.Fill(tdiff);
        if ( (tdiff > -4)&&(tdiff < 4) ){
            coincident_gammas.push_back(labr_event);
            exgam.Fill(labr_event.energy, excitation);
        } else if ( (tdiff - 58.5 > -4)&&(tdiff -58.5 < 4) ){
            exgam_bg.Fill(labr_event.energy, excitation);
        }
    }

    if ( coincident_gammas.size() == 2 ){
        auto max = ( coincident_gammas[0].energy > coincident_gammas[1].energy) ? coincident_gammas[0].energy : coincident_gammas[1].energy;
        auto min = ( coincident_gammas[0].energy < coincident_gammas[1].energy) ? coincident_gammas[0].energy : coincident_gammas[1].energy;
        if ( (excitation > 4650) && (excitation < 4950) )
            gamgam.Fill(min, max);
    }
    
    double total_gamma_energy = 0;
    for ( auto& entry : coincident_gammas ) {
        total_gamma_energy += entry.energy;
    }

    sum_ex_gam.Fill(excitation - total_gamma_energy);

    if ( (excitation - total_gamma_energy > -200) && (excitation - total_gamma_energy < 350) ){
        for ( auto &evt : coincident_gammas )
            exgam_full.Fill(evt.energy, excitation);
    }

    std::sort(coincident_gammas.begin(), coincident_gammas.end(),
              [](const Entry_t &lhs, const Entry_t &rhs){ return lhs.energy > rhs.energy; });




    if ( coincident_gammas.size() >= 3 ){
        if ((excitation > 4500) && (excitation < 5000)) {
            bool have_2214 = false;
            bool have_1345 = false;
            for ( auto &evt : coincident_gammas ){
                if ( (evt.energy > 2214 - 15) && (evt.energy < 2214 + 15) ) {
                    have_2214 = true;
                } else if ( (evt.energy > 1345 - 15) && (evt.energy < 1345 + 15) ){
                    have_1345 = true;
                }
            }
            if ( (have_2214&&have_1345) ){
                for ( auto &evt : coincident_gammas ){
                    if ( ((evt.energy > 2214 - 15) && (evt.energy < 2214 + 15)) ||
                         ((evt.energy > 1345 - 15) && (evt.energy < 1345 + 15)) )
                        continue;
                    part_gamgam.Fill(evt.energy);
                }
            }
        }
    }

}

void ParticleCoincidence::Flush()
{
    de_thickness.force_flush();
    particle_energy.force_flush();
    excitation_energy.force_flush();
    exgam.force_flush();
    exgam_full.force_flush();
    gamgam.force_flush();
    part_gamgam.force_flush();
    sum_ex_gam.force_flush();
    gam_part_time.force_flush();
    std::cout << "Average thickness: " << thickness << std::endl;
    std::cout << "Counts: " << counts << std::endl;
}