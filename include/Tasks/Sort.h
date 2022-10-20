//
// Created by Vetle Wegner Ingeberg on 16/04/2021.
//

#ifndef TDR2TREE_SORT_H
#define TDR2TREE_SORT_H

#include <Task.h>

#include <histogram/Histogram1D.h>
#include <histogram/Histogram2D.h>
#include <histogram/Histograms.h>
#include <histogram/ThreadSafeHistograms.h>

#include <experimentsetup.h>
#include <CommandLineInterface.h>
#include "Queue.h"
#include "event.h"


namespace Task {

    struct Detector_Histograms_t
    {
        Histogram2Dp time;
        Histogram2Dp energy;
        Histogram2Dp energy_cal;
        Histogram1Dp mult;

        Detector_Histograms_t(Histograms &hist, const std::string &name, const size_t &num);

        void Fill(const Entry_t &word);
        void Fill(const subvector<Entry_t> &subvec,
                  const Entry_t *start = nullptr);

    };

    struct Particle_telescope_t
    {
        double lhs, rhs;
        Histogram2Dp time_de_energy;
        Histogram2Dp time_e_energy;
        Histogram2Dp ede_spectra[NUM_SI_DE_TEL];
        Histogram2Dp particle_range;

        Particle_telescope_t(Histograms &hist, const size_t &num, const double &lhs, const double &rhs);

        void Fill(const subvector<Entry_t> &deltaE, const subvector<Entry_t> &E);
    };

    class UserSort {
    public:
        virtual ~UserSort() = default;
        virtual void FillEvent(const Triggered_event &event) = 0;
    };

    class DynamicLibrary {
    private:
        void *_handle;
    public:
        DynamicLibrary() = default;
        DynamicLibrary(const char *libname);
        ~DynamicLibrary();

        UserSort *GetUserSort(Histograms *hist);

    };

    class UserSortManager
    {
    private:
        DynamicLibrary library;
        UserSort *object;

    public:
        UserSortManager(Histograms &hist, const char *libname = nullptr);
        ~UserSortManager();
        void FillEvent(const Triggered_event &event);
    };

    class HistManager {

    private:

        Histograms histograms;

        Detector_Histograms_t labr;
        Detector_Histograms_t si_de;
        Detector_Histograms_t si_e;
        Detector_Histograms_t ppacs;

        //! Time energy spectra for particles.
        Particle_telescope_t particle_coincidence[NUM_SI_DE_TEL]; // Sorted by ring number

        Detector_Histograms_t *GetSpec(const DetectorType &type);
        inline Particle_telescope_t *GetPart(const size_t &num){ return ( num < NUM_SI_DE_TEL ) ? particle_coincidence+num : nullptr; }

        UserSortManager usersort;

    public:
        HistManager();
        ~HistManager() = default;
        //HistManager(Histograms &histograms);

        inline Histograms &GetHistograms(){ return histograms; }

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
    };

    class Sort : public Base
    {
    private:
        TEventQueue_t &input_queue;
        HistManager hm;

    public:
        Sort(TEventQueue_t &input);
        Histograms &GetHistograms(){ return hm.GetHistograms(); }
        void Run() override;

    };

    class Sorters {
    private:
        TEventQueue_t &input_queue;
        ThreadSafeHistograms histograms;
        std::vector<Sort *> sorters;

    public:
        Sorters(TEventQueue_t &input, const CLI::Options &options, const size_t &no_workers = 4);
        ~Sorters();

        std::vector<Sort *>::iterator begin(){ return sorters.begin(); }
        std::vector<Sort *>::iterator end(){ return sorters.end(); }

        Histograms &GetHistogram(){ return histograms.GetHistograms(); }

    };

    /*!
     * Gather and merge histograms.
     * \param sorters
     * \param outfile
     */
    //extern void Gather(std::vector<Sort> &sorters, const char *outfile);

}

#endif //TDR2TREE_SORT_H
