//
// Created by Vetle Wegner Ingeberg on 20/10/2022.
//

#include "MTSort.h"

#include <TFile.h>
#include <TTree.h>

#ifndef MAX_ENTRIES
#define MAX_ENTRIES 192
#endif // MAX_ENTRIES

using namespace Task;

std::string BranchName(const char *name, const char *varname)
{
    return std::string(name) + "_" + std::string(varname);
}

std::string LeafList(const char *name, const char *varname, const bool &mult, const char &type)
{
    std::string result;
    if ( mult ) {
        result.reserve(snprintf(nullptr, 0, "%s%s[%s%s]/%c", name, varname, name, "Mult", type) + 1);
        snprintf(result.data(), result.capacity(), "%s%s[%s%s]/%c", name, varname, name, "Mult", type);
    } else {
        result.reserve(snprintf(nullptr, 0, "%s%s/%c", name, varname, type) + 1);
        snprintf(result.data(), result.capacity(), "%s%s/%c", name, varname, type);
    }
    return result;
}

template<typename T>
TBranch *make_leaf(TTree *tree, T *addr, const char *base_name, const char *varname, const bool &mult, const char &type)
{
    return tree->Branch(BranchName(base_name, varname).c_str(),
                        addr,
                        LeafList(base_name, varname, mult, type).c_str());
}

namespace Task {

    struct TriggerEntry {
        // This will contain the trigger event data such as
        unsigned short ID;
        bool finishflag;
        unsigned short adcvalue;
        long long timestamp;
        double energy;
        bool cfdfail;
        double cfdcorr;
        unsigned short idx;     //!< Index in list of detectors that the trigger corresponds to.

        TriggerEntry(TTree &tree){
            make_leaf(&tree, &ID, "Trigger", "ID", false, 's');
            make_leaf(&tree, &finishflag, "Trigger", "FinishFlag", false, 'O');
            make_leaf(&tree, &adcvalue, "Trigger", "ADCValue", false, 's');
            make_leaf(&tree, &timestamp, "Trigger", "timestamp", false, 'L');
            make_leaf(&tree, &energy, "Trigger", "energy", false, 'D');
            make_leaf(&tree, &cfdfail, "Trigger", "CFDfail", false, 'O');
            make_leaf(&tree, &cfdcorr, "Trigger", "CFDcorr", false, 'D');
            make_leaf(&tree, &idx, "Trigger", "IDX", false, 's');
        }

        void Fill(const Entry_t *trigger){
            ID = trigger->detectorID;
            finishflag = trigger->finishflag;
            adcvalue = trigger->adcvalue;
            timestamp = trigger->timestamp;
            energy = trigger->energy;
            cfdfail = trigger->cfdfail;
            cfdcorr = trigger->cfdcorr;
        }
    };

    struct DetectorEntries {
        unsigned short mult;    //!< Number of entries of the detector type in event    */
        unsigned short ID[MAX_ENTRIES]; //!< ID number of the detector event.           */
        bool finishflag[MAX_ENTRIES];   //!< Pile-up flag   */
        unsigned short adcvalue[MAX_ENTRIES]; //!< 16-bit ADC reading */
        //unsigned short cfdvalue[MAX_ENTRIES]; //!< 16-bit CFD result (obmitted for now since this should not be done afterwards...) */
        long long timestamp[MAX_ENTRIES];   //!< Timestamp in ns    */
        double energy[MAX_ENTRIES]; //!< Energy of the event    */
        double time[MAX_ENTRIES];   //!< Time w.r.t. trigger    */
        bool cfdfail[MAX_ENTRIES];  //!< Result of the CFD filter   */
        double cfdcorr[MAX_ENTRIES]; //!< CFD correction to the timestamp    */

        void Fill(const subvector<Entry_t> &entries, const Entry_t *trigger = nullptr){
            mult = 0;
            for ( auto &entry : entries ){
                ID[mult] = entry.detectorID;
                finishflag[mult] = entry.finishflag;
                adcvalue[mult] = entry.adcvalue;
                timestamp[mult] = entry.timestamp;
                energy[mult] = entry.energy;

                if ( trigger ){
                    time[mult] = double(entry.timestamp - trigger->timestamp);
                    time[mult] += entry.cfdcorr - trigger->cfdcorr;
                } else {
                    time[mult] = 0;
                }
                cfdfail[mult] = entry.cfdfail;
                cfdcorr[mult++] = entry.cfdcorr;
            }
        }

        DetectorEntries(TTree &tree, const char *base_name)
            : mult( 0 )
        {
            make_leaf(&tree, &mult, base_name, "Mult", false, 's');
            make_leaf(&tree, ID, base_name, "ID", true, 's');
            make_leaf(&tree, finishflag, base_name, "FinishFlag", true, 'O');
            make_leaf(&tree, adcvalue, base_name, "ADCValue", true, 's');
            make_leaf(&tree, timestamp, base_name, "timestamp", true, 'L');
            make_leaf(&tree, energy, base_name, "energy", true, 'D');
            make_leaf(&tree, cfdfail, base_name, "CFDfail", true, 'O');
            make_leaf(&tree, cfdcorr, base_name, "CFDcorr", true, 'D');
        }

    };

    class MT_Safe_TTree_Container {
    private:
        class spinlock {
        private:
            std::atomic_flag atomic_flag = ATOMIC_FLAG_INIT;
        public:
            void lock(){ while( atomic_flag.test_and_set(std::memory_order_acquire) ){} }
            void unlock(){ atomic_flag.clear(std::memory_order_release); }
        };

        template<typename T>
        class Lock {
        private:
            T &_lock;
        public:
            Lock(T &lock) : _lock( lock ) { _lock.lock(); }
            ~Lock(){ _lock.unlock(); }
        };
    private:
        TFile file;
        TTree tree;
        spinlock lock;

        TriggerEntry trigger;
        DetectorEntries deDet;
        DetectorEntries eDet;
        DetectorEntries ppacDet;
        DetectorEntries labrDet;

    public:


        explicit MT_Safe_TTree_Container(const char *fname)
            : file( fname, "RECREATE" )
            , tree( "ocl_events", "OCL events" )
            , trigger( tree )
            , deDet( tree, "deDet" )
            , eDet( tree, "eDet" )
            , ppacDet( tree, "ppac" )
            , labrDet( tree, "labr" )
        {}

        ~MT_Safe_TTree_Container(){
            tree.Write();
            file.Write();
            file.Close();
        }

        void Fill(const Triggered_event &event)
        {
            // Get the lock...
            Lock l(lock);
            if ( event.GetTrigger() )
                trigger.Fill(event.GetTrigger());
            deDet.Fill(event.GetDetector(DetectorType::deDet), event.GetTrigger());
            eDet.Fill(event.GetDetector(DetectorType::eDet), event.GetTrigger());
            ppacDet.Fill(event.GetDetector(DetectorType::ppac), event.GetTrigger());
            labrDet.Fill(event.GetDetector(DetectorType::labr), event.GetTrigger());
            tree.Fill();
        }

    };
}


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

    // For now, we will discard events with bad CFD
    if ( trigger->cfdfail )
        return;

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

MTSort::MTSort(Task::TEventQueue_t &input,ThreadSafeHistograms &histograms,
               MT_Safe_TTree_Container *tree_container, const char *custom_sort)
    : input_queue( input )
    , hm( histograms, custom_sort )
    , tree( tree_container )
{
}

void MTSort::Run()
{
    std::pair<std::vector<Entry_t>, size_t> entries;
    while ( !done ){
        if ( input_queue.wait_dequeue_timed(entries, std::chrono::seconds(1)) ){
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

Sorters::Sorters(TEventQueue_t &input, const char *tree_name, const char *_user_sort)
    : input_queue( input )
    , histograms( )
    , sorters( )
    , user_sort_path( ( _user_sort ) ? _user_sort : "" )
    , tree_file_name( tree_name )
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
    }
    sorters.push_back(new MTSort(input_queue, histograms, (), (user_sort_path.empty()) ? nullptr : user_sort_path.c_str()));
    sorters.push_back(new MTSort(input_queue, histograms, tree_container, (user_sort_path.empty()) ? nullptr : user_sort_path.c_str()));
    return sorters.back();
}