//
// Created by Vetle Wegner Ingeberg on 20/10/2022.
//

#ifndef XIA2TREE_MTSORT_H
#define XIA2TREE_MTSORT_H

#include <histogram/ThreadSafeHistograms.h>
#include <UserSort/UserSortManager.h>

#include "Task.h"
#include "Queue.h"
#include "event.h"

namespace Task {

    struct MT_Detector_Histograms_t
    {
        ThreadSafeHistogram2D time;
        ThreadSafeHistogram2D energy;
        ThreadSafeHistogram2D energy_cal;
        ThreadSafeHistogram1D mult;

        MT_Detector_Histograms_t(ThreadSafeHistograms &hist, const std::string &name, const size_t &num);

        void Fill(const Entry_t &word);
        void Fill(const subvector<Entry_t> &subvec,
                  const Entry_t *start = nullptr);
        void Flush();

    };

    struct MT_Particle_telescope_t
    {
        double lhs, rhs;
        ThreadSafeHistogram2D time_de_energy;
        ThreadSafeHistogram2D time_e_energy;
        ThreadSafeHistogram2D ede_spectra[NUM_SI_DE_TEL];
        ThreadSafeHistogram2D particle_range;

        MT_Particle_telescope_t(ThreadSafeHistograms &hist, const size_t &num, const double &lhs, const double &rhs);

        void Fill(const subvector<Entry_t> &deltaE, const subvector<Entry_t> &E);
        void Flush();
    };

    class MTHistManager {
        MT_Detector_Histograms_t labr;
        MT_Detector_Histograms_t si_de;
        MT_Detector_Histograms_t si_e;
        MT_Detector_Histograms_t ppacs;

        //! Time energy spectra for particles.
        MT_Particle_telescope_t particle_coincidence[NUM_SI_DE_TEL]; // Sorted by ring number

        UserSortManager userSort;

        MT_Detector_Histograms_t *GetSpec(const DetectorType &type);
        inline MT_Particle_telescope_t *GetPart(const size_t &num){ return ( num < NUM_SI_DE_TEL ) ? particle_coincidence+num : nullptr; }

    public:
        MTHistManager(ThreadSafeHistograms &histograms, const char *custom_sort = nullptr);
        ~MTHistManager() = default;
        //HistManager(Histograms &histograms);

        //! Fill spectra with an event
        void AddEntry(Triggered_event &buffer);

        //! Fill a single word
        //void AddEntry(const Entry_t &word);

        //! Fill spectra directly from iterators
        template<class It>
        inline void AddEntries(It start, It stop){
            using std::placeholders::_1;
            std::for_each(start, stop, [this](const auto &p){ this->AddEntry(p); });
        }

        void Flush();
    };

    class MTSort  : public Base
    {
    private:
        TEventQueue_t &input_queue;
        MTHistManager hm;

    public:
        MTSort(TEventQueue_t &input, ThreadSafeHistograms &histograms, const char *custom_sort = nullptr);
        ~MTSort() = default;
        void Run() override;
        void Flush();
    };

    class Sorters
    {
    private:
        TEventQueue_t &input_queue;
        ThreadSafeHistograms histograms;
        std::vector<MTSort *> sorters;
        std::string user_sort_path;
    public:
        Sorters(TEventQueue_t &input, const char *user_sort = nullptr);
        ~Sorters();
        void flush();
        Histograms &GetHistograms(){
            flush();
            return histograms.GetHistograms();
        }

        MTSort *GetNewSorter();
    };

}

#endif //XIA2TREE_MTSORT_H
