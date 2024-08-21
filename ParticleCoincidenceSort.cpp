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
    const OCL::UserConfiguration *user_config;
    double thickness;
    long long counts;

public:
    ParticleConicidence(ThreadSafeHistograms *hist, const OCL::UserConfiguration *user_config);
    ~ParticleConicidence() = default;
    void FillEvent(const Triggered_event &event) override;
    void Flush() override;

};

extern "C" {
    UserSort *NewUserSort(ThreadSafeHistograms *hist, const OCL::UserConfiguration *config){
        return new ParticleConicidence(hist, config);
    }
}

ParticleConicidence::ParticleConicidence(ThreadSafeHistograms *hist, const OCL::UserConfiguration *config)
    : user_config( config )
    , thickness( 0 )
    , counts( 0 )
{

}

void ParticleConicidence::FillEvent(const Triggered_event &event)
{

    auto [deDet, eDet] = event.GetTrap(event.GetTrigger()->detectorID);

    if ( eDet.size() != 1 || deDet.size() != 1 )
        return;

    for ( auto &de : deDet ){
        for ( auto &e : eDet ){
            thickness += user_config->GetRange().GetRange((de.energy + e.energy)*1e-3) - user_config->GetRange().GetRange(e.energy*1e-3);
            counts += 1;
        }
    }

}

void ParticleConicidence::Flush()
{
    std::cout << "Average thickness: " << thickness << std::endl;
    std::cout << "Counts: " << counts << std::endl;
}