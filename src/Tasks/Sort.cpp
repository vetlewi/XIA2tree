//
// Created by Vetle Wegner Ingeberg on 16/04/2021.
//

#include "Sort.h"

#include "PhysicalParam/Parameters.h"

#include <dlfcn.h>

using namespace Task;

DynamicLibrary::DynamicLibrary(const char *libname)
    : _handle( dlopen(libname, RTLD_LAZY) )
{
    if ( !_handle && libname ){ // Only error if the library name is not a nullptr.
        std::cerr << "Unable to load '" << libname << ". Got error '";
        std::cerr << dlerror() << "'." << std::endl;
    }
}

DynamicLibrary::~DynamicLibrary()
{
    if ( _handle )
        dlclose(_handle);
    _handle = nullptr;
}

UserSort *DynamicLibrary::GetUserSort(Histograms *hist)
{
    if ( !_handle ){
        return nullptr; // It is completely fine if we don't have anything. Skip if not present.
    }
    using Func = UserSort*(*)(Histograms *);
    auto sym = reinterpret_cast<Func>(dlsym(_handle, "NewUserSort"));
    if ( !sym ){
        std::cerr << "Could not load NewUserSort. Got error '" << dlerror() << std::endl;
        return nullptr;
    }
    return sym(hist);
}

UserSortManager::UserSortManager(Histograms &hist, const char *libname)
    : library( libname )
    , object( library.GetUserSort(&hist) )
{
}

UserSortManager::~UserSortManager()
{
    delete object;
}

void UserSortManager::FillEvent(const Triggered_event &event)
{
    if ( object )
        object->FillEvent(event);
}

Detector_Histograms_t::Detector_Histograms_t(Histograms &hm, const std::string &name, const size_t &num)
        : time( hm.Create2D(std::string("time_"+name), std::string("Time spectra "+name), 30000, -1500, 1500, "Time [ns]", num, 0, num, std::string(name+" ID").c_str()) )
        , energy( hm.Create2D(std::string("energy_"+name), std::string("Energy spectra "+name), 65536, 0, 65536, "Energy [ch]", num, 0, num, std::string(name+" ID").c_str()) )
        , energy_cal( hm.Create2D(std::string("energy_cal_"+name), std::string("energy spectra "+name+" (cal)"), 16384, 0, 16384, "Energy [keV]", num, 0, num, std::string(name+" ID").c_str()) )
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
        /*if ( start ){
            if ( start != &entry ){
                time->Fill(double(entry.timestamp - start->timestamp) + (entry.cfdcorr - start->cfdcorr), entry.detectorID);
            }
        }*/
        if ( start && !( (entry.type == start->type)&&(entry.detectorID == start->detectorID)&&(entry.timestamp==start->timestamp) ))
            time->Fill(double(entry.timestamp - start->timestamp) + (entry.cfdcorr - start->cfdcorr), entry.detectorID);
        //if ( start )

    }
}

Particle_telescope_t::Particle_telescope_t(Histograms &hm, const size_t &num, const double &_lhs, const double &_rhs)
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

void Particle_telescope_t::Fill(const subvector<Entry_t> &deltaE, const subvector<Entry_t> &E)
{
    double timediff;
    for ( auto &de : deltaE ){
        for ( auto &e : E ){

            // Check if the same trapzoid. If not, then we will just skip.
            if ( de.detectorID / NUM_SI_DE_TEL != e.detectorID )
                continue;

            timediff = double(de.timestamp - e.timestamp) + (de.cfdcorr - e.cfdcorr);
            time_de_energy->Fill(de.energy, timediff);
            time_e_energy->Fill(e.energy, timediff);
            ede_spectra[de.detectorID % NUM_SI_DE_TEL]->Fill(e.energy, de.energy);

        }
    }
}

HistManager::HistManager()
    : histograms( )
    , labr( histograms, "labr", NUM_LABR_DETECTORS )
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
   , usersort( histograms, "libParticleCoincidenceSort.so" )
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

    for ( auto &type : {DetectorType::labr, DetectorType::deDet, DetectorType::eDet, DetectorType::ppac} ){
        GetSpec(type)->Fill(buffer.GetDetector(type), trigger);
    }

    // Next we will get by ring ID
    for ( auto &i : {0, 1, 2, 3, 4, 5, 6, 7}){
        GetPart(i)->Fill(buffer.GetRing(i), buffer.GetDetector(eDet));
    }

    usersort.FillEvent(buffer);
}

Sort::Sort(Task::TEventQueue_t &input)
    : input_queue( input )
    , hm( )
{
}

void Sort::Run()
{
    std::pair<std::vector<Entry_t>, size_t> entries;
    while ( !done ){
        if ( input_queue.wait_dequeue_timed(entries, std::chrono::seconds(1)) ){
            Triggered_event event(entries.first, entries.first[entries.second]);
            hm.AddEntry(event);
        }
    }
    is_done = true;
}