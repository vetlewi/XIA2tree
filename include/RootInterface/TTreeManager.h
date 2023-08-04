//
// Created by Vetle Wegner Ingeberg on 04/04/2023.
//

#ifndef TTREEMANAGER_H
#define TTREEMANAGER_H

#include <TFile.h>
#include <TTree.h>

#include <entry.h>
#include <event.h>

#ifndef MAX_ENTRIES
#define MAX_ENTRIES 128
#endif // MAX_ENTRIES

namespace Task {

    namespace details {
        class TriggerEntry {
        private:
            unsigned short ID;
            bool finishflag;
            unsigned short adcvalue;
            long long timestamp;
            double energy;
            bool cfdfail;
            double cfdcorr;
            unsigned short idx;     //!< Index in list of detectors that the trigger corresponds to.

        public:
            explicit TriggerEntry(TTree &tree);

            inline void Fill(const Entry_t *trigger)
            {
                ID = trigger->detectorID;
                finishflag = trigger->finishflag;
                adcvalue = trigger->adcvalue;
                timestamp = trigger->timestamp;
                energy = trigger->energy;
                cfdfail = trigger->cfdfail;
                cfdcorr = trigger->cfdcorr;
            }
        };

        class DetectorEntries
        {
        private:
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

        public:
            DetectorEntries(TTree &tree, const char *base_name);


            inline void Fill(const subvector<Entry_t> &entries, const Entry_t *trigger = nullptr){
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

            inline void reset(){ mult = 0; }
        };

    }

    namespace ROOT {
        class TTreeManager {
        private:
            TFile file;
            TTree tree;

            details::TriggerEntry trigger;
            details::DetectorEntries deDet;
            details::DetectorEntries eDet;
            details::DetectorEntries ppacDet;
            details::DetectorEntries labrDet;

            details::DetectorEntries *GetDet(const DetectorType &type){
                switch ( type ) {
                    case DetectorType::labr : return &labrDet;
                    case DetectorType::deDet : return &deDet;
                    case DetectorType::eDet : return &eDet;
                    case DetectorType::ppac : return &ppacDet;
                    default : return nullptr;
                }
            }

        public:


            explicit TTreeManager(const char *fname)
                    : file( fname, "RECREATE" )
                    , tree( "ocl_events", "OCL events" )
                    , trigger( tree )
                    , deDet( tree, "deDet" )
                    , eDet( tree, "eDet" )
                    , ppacDet( tree, "ppac" )
                    , labrDet( tree, "labr" )
            {}

            ~TTreeManager(){
                tree.Write();
                file.Write();
                file.Close();
            }

            inline void Fill(const Triggered_event &event)
            {
                // Get the lock...
                if ( event.GetTrigger() )
                    trigger.Fill(event.GetTrigger());
                for ( auto &type : {DetectorType::labr, /*DetectorType::deDet,*/ DetectorType::eDet, DetectorType::ppac} ){
                    GetDet(type)->reset();
                    GetDet(type)->Fill(event.GetDetector(type), event.GetTrigger());
                }
                GetDet(DetectorType::deDet)->reset();
                GetDet(DetectorType::deDet)->Fill(event.GetDetector(DetectorType::deDet), event.GetTrigger());

                tree.Fill();
            }

        };
    }
}



#endif // TTREEMANAGER_H
