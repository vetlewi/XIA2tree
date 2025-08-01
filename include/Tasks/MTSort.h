//
// Created by Vetle Wegner Ingeberg on 20/10/2022.
//

#ifndef XIA2TREE_MTSORT_H
#define XIA2TREE_MTSORT_H


#include <histogram/ThreadSafeHistograms.h>
#include <UserSort/UserSortManager.h>

#include "ConfigManager.h"

#include "Task.h"
#include "Queue.h"
#include "event.h"

class ParticleRange;

namespace Task {

    namespace ROOT {
        class TTreeManager;
    }

    struct Detector_Histograms_t
    {
        ThreadSafeHistogram2D time;
        ThreadSafeHistogram2D time_CFDfail;
        ThreadSafeHistogram2D energy;
        ThreadSafeHistogram2D energy_cal;
        ThreadSafeHistogram1D mult;

        Detector_Histograms_t(ThreadSafeHistograms &hist, const std::string &name, const size_t &num);

        void Fill(const Entry_t &word);
        void Fill(const subvector<Entry_t> &subvec,
                  const Entry_t *start = nullptr);
        void Flush();

    };

    struct Particle_telescope_t
    {
        ThreadSafeHistogram2D ede_spectra[NUM_SI_DE_TEL];
        ThreadSafeHistogram2D ede_spectra_raw[NUM_SI_DE_TEL];

        Particle_telescope_t(ThreadSafeHistograms &hist, const size_t &num);
        void Fill(const subvector<Entry_t> &deltaE, const subvector<Entry_t> &E);
        void Fill(const std::vector<Entry_t> &deltaE, const std::vector<Entry_t> &E);
        void Flush();
    };

    class HistManager {
    private:
        const OCL::UserConfiguration configuration;

        Detector_Histograms_t labr;
        Detector_Histograms_t si_de;
        Detector_Histograms_t si_e;
        Detector_Histograms_t ppacs;

        //! Time energy spectra for particles.
        Particle_telescope_t particle_coincidence[NUM_SI_E_DET]; // Sorted by back number

        ThreadSafeHistogram2D ede_spectra[NUM_SI_DE_TEL];
        ThreadSafeHistogram2D ede_time;
        ThreadSafeHistogram2D thickness;
        ThreadSafeHistogram2D particle_energy;
        ThreadSafeHistogram2D alfna_prompt, alfna_background;
        ThreadSafeHistogram2D ts_ex_above_Sn;
        ThreadSafeHistogram2D mult_ex;

        UserSortManager userSort;

        Detector_Histograms_t *GetSpec(const DetectorType &type);
        inline Particle_telescope_t *GetPart(const size_t &num){ return ( num < NUM_SI_DE_TEL ) ? particle_coincidence+num : nullptr; }

    public:
        HistManager(ThreadSafeHistograms &histograms, const OCL::UserConfiguration &configuration,
                    const char *custom_sort = nullptr);
        ~HistManager() = default;

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
        HistManager hm;
        std::unique_ptr<ROOT::TTreeManager> tree;

    public:
        MTSort(TEventQueue_t &input, ThreadSafeHistograms &histograms, const OCL::UserConfiguration &config,
               const char *tree_name = nullptr, const char *user_sort = nullptr);
        ~MTSort() override = default;
        void Run() override;
        void Flush();
    };

    class Sorters
    {
    private:
        TEventQueue_t &input_queue;
        ThreadSafeHistograms histograms;
        std::vector<MTSort *> sorters;
        const OCL::UserConfiguration &user_config;
        std::string user_sort_path;
        std::string tree_file_name;
        std::vector<std::string> tree_files; //! To be returned to the user when everything is said and done.

    public:
        Sorters(TEventQueue_t &input, OCL::UserConfiguration &config, const char *tree_name = nullptr, const char *user_sort = nullptr);
        ~Sorters();
        void flush();
        Histograms &GetHistograms(){
            flush();
            return histograms.GetHistograms();
        }
        [[nodiscard]] std::vector<std::string> GetTreeFiles() const { return tree_files; }
        MTSort *GetNewSorter();
    };

}

#endif //XIA2TREE_MTSORT_H
