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
    , excitation_energy( hist->Create2D("excitation", "Ex energy", 10000, 0, 10000, "Excitation energy [keV]", 8, 8, 8, "Ring #") )
{

}

void ParticleCoincidence::FillEvent(const Triggered_event &event)
{

    auto [deDet, eDet] = event.GetTrap(event.GetTrigger()->detectorID);

    if ( eDet.size() != 1 || deDet.size() != 1 )
        return;

    for ( auto &de : deDet ){
        for ( auto &e : eDet ){
            double thick = user_config->GetRange().GetRange((de.energy + e.energy)*1e-3) - user_config->GetRange().GetRange(e.energy*1e-3);
            thickness += thick;
            counts += 1;
            de_thickness.Fill(thick, de.detectorID % 8);
        }
    }
}

void ParticleCoincidence::Flush()
{
    de_thickness.force_flush();
    particle_energy.force_flush();
    excitation_energy.force_flush();
    std::cout << "Average thickness: " << thickness << std::endl;
    std::cout << "Counts: " << counts << std::endl;
}