//
// Created by Vetle Wegner Ingeberg on 20/10/2022.
//

#include "MTSort.h"

using namespace Task;


MT_Detector_Histograms_t::MT_Detector_Histograms_t(ThreadSafeHistograms &hm, const std::string &name, const size_t &num)
    : time( hm.Create2D(std::string("time_"+name), std::string("Time spectra "+name), 30000, -1500, 1500, "Time [ns]", num, 0, num, std::string(name+" ID")) )
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
        if ( start && !( (entry.type == start->type)&&(entry.detectorID == start->detectorID)&&(entry.timestamp==start->timestamp) ))
            time.Fill(double(entry.timestamp - start->timestamp) + (entry.cfdcorr - start->cfdcorr), entry.detectorID);
        //if ( start )

    }
}

void MT_Detector_Histograms_t::Flush()
{
    time.force_flush();
    energy.force_flush();
    energy_cal.force_flush();
    mult.force_flush();
}

MT_Particle_telescope_t::MT_Particle_telescope_t(ThreadSafeHistograms &hm, const size_t &num, const double &_lhs, const double &_rhs)
        : lhs( _lhs ), rhs( _rhs )
        , time_de_energy( hm.Create2D("time_de_energy_"+std::to_string(num), "Time vs dE energy",
                                      1000, 0, 10000, "Energy [keV]",
                                      3000, -1500, 1500, "Time [ns]"))//,
        //std::string("trap_")+std::to_string(num)) )
        , time_e_energy( hm.Create2D("time_e_energy_"+std::to_string(num), "Time vs E energy",
                                     1000, 0, 20000, "Energy [keV]",
                                     3000, -1500, 1500, "Time [ns]"))//,
        //std::string("trap_")+std::to_string(num)) )
        , ede_spectra{ hm.Create2D("ede_spectra_b"+std::to_string(num)+"f0", "E energy vs dE energy",
                                   1000, 0, 20000, "E energy [keV]", 1000, 0, 10000, "dE energy [keV]"),
                //std::string("trap_")+std::to_string(num)),
                       hm.Create2D("ede_spectra_b"+std::to_string(num)+"f1", "E energy vs dE energy",
                                   1000, 0, 20000, "E energy [keV]", 1000, 0, 10000, "dE energy [keV]"),
                //std::string("trap_")+std::to_string(num)),
                       hm.Create2D("ede_spectra_b"+std::to_string(num)+"f2", "E energy vs dE energy",
                                   1000, 0, 20000, "E energy [keV]", 1000, 0, 10000, "dE energy [keV]"),
                //std::string("trap_")+std::to_string(num)),
                       hm.Create2D("ede_spectra_b"+std::to_string(num)+"f3", "E energy vs dE energy",
                                   1000, 0, 20000, "E energy [keV]", 1000, 0, 10000, "dE energy [keV]"),
                //std::string("trap_")+std::to_string(num)),
                       hm.Create2D("ede_spectra_b"+std::to_string(num)+"f4", "E energy vs dE energy",
                                   1000, 0, 20000, "E energy [keV]", 1000, 0, 10000, "dE energy [keV]"),
                //std::string("trap_")+std::to_string(num)),
                       hm.Create2D("ede_spectra_b"+std::to_string(num)+"f5", "E energy vs dE energy",
                                   1000, 0, 20000, "E energy [keV]", 1000, 0, 10000, "dE energy [keV]"),
                //std::string("trap_")+std::to_string(num)),
                       hm.Create2D("ede_spectra_b"+std::to_string(num)+"f6", "E energy vs dE energy",
                                   1000, 0, 20000, "E energy [keV]", 1000, 0, 10000, "dE energy [keV]"),
                //std::string("trap_")+std::to_string(num)),
                       hm.Create2D("ede_spectra_b"+std::to_string(num)+"f7", "E energy vs dE energy",
                                   1000, 0, 20000, "E energy [keV]", 1000, 0, 10000, "dE energy [keV]")}
        //std::string("trap_")+std::to_string(num))}
        , particle_range( hm.Create2D("particle_range_"+std::to_string(num), "Particle range",
                                      2000, 0, 2000, "Range [um]", 8, 0, 8, "Ring ID"))//,
//std::string("trap_")+std::to_string(num)) )
{
}

void MT_Particle_telescope_t::Fill(const subvector<Entry_t> &deltaE, const subvector<Entry_t> &E)
{
    double timediff;
    for ( auto &de : deltaE ){
        for ( auto &e : E ){

            // Check if the same trapzoid. If not, then we will just skip.
            if ( de.detectorID / NUM_SI_DE_TEL != e.detectorID )
                continue;

            timediff = double(de.timestamp - e.timestamp) + (de.cfdcorr - e.cfdcorr);
            time_de_energy.Fill(de.energy, timediff);
            time_e_energy.Fill(e.energy, timediff);
            ede_spectra[de.detectorID % NUM_SI_DE_TEL].Fill(e.energy, de.energy);

        }
    }
}

void MT_Particle_telescope_t::Flush()
{
    time_de_energy.force_flush();
    time_e_energy.force_flush();
    for ( auto &ede : ede_spectra ){
        ede.force_flush();
    }
    particle_range.force_flush();
}

MTHistManager::MTHistManager(ThreadSafeHistograms &histograms, const char *custom_sort)
        : labr( histograms, "labr", NUM_LABR_DETECTORS )
        , si_de( histograms, "si_de", NUM_SI_DE_DET )
        , si_e( histograms, "si_e", NUM_SI_E_DET )
        , ppacs( histograms, "ppac", NUM_PPAC )
        , particle_coincidence{{ histograms, 0, -100, 100},
                               { histograms, 1, -100, 100},
                               { histograms, 2, -100, 100},
                               { histograms, 3, -100, 100},
                               { histograms, 4, -100, 100},
                               { histograms, 5, -100, 100},
                               { histograms, 6, -100, 100},
                               { histograms, 7, -100, 100}}
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

    for ( auto &type : {DetectorType::labr, DetectorType::deDet, DetectorType::eDet, DetectorType::ppac} ){
        GetSpec(type)->Fill(buffer.GetDetector(type), trigger);
    }

    // Next we will get by ring ID
    for ( auto &i : {0, 1, 2, 3, 4, 5, 6, 7}){
        GetPart(i)->Fill(buffer.GetRing(i), buffer.GetDetector(eDet));
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

MTSort::MTSort(Task::TEventQueue_t &input,ThreadSafeHistograms &histograms, const char *custom_sort)
    : input_queue( input )
    , hm( histograms, custom_sort )
{
}

void MTSort::Run()
{
    std::pair<std::vector<Entry_t>, size_t> entries;
    while ( !done ){
        if ( input_queue.wait_dequeue_timed(entries, std::chrono::seconds(1)) ){
            Triggered_event event(entries.first, entries.first[entries.second]);
            hm.AddEntry(event);
        }
    }
    is_done = true;
    Flush();
}

void MTSort::Flush()
{
    hm.Flush();
}

Sorters::Sorters(TEventQueue_t &input, const char *_user_sort)
    : input_queue( input )
    , histograms( )
    , sorters( )
    , user_sort_path( ( _user_sort ) ? _user_sort : "" )
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
    sorters.push_back(new MTSort(input_queue, histograms, (user_sort_path.empty()) ? nullptr : user_sort_path.c_str()));
    return sorters.back();
}