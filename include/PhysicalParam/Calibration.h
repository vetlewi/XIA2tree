#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <cstdint>
#include <array>

#include "PhysicalParam/experimentsetup.h"
#include "PhysicalParam/XIA_CFD.h"
#include "Format/xiaformat.h"
#include "Format/entry.h"
// We know how many crates, modules and channels there are.
// We can easily populate the calibration scheme from a json
// file. This would be ideal since this is an easy editable
// format easily readable.
// We will also use this class to store information about the
// channel mappings.

//#define NUM_ADDRESSABLE 4096 // Total number of addressable channels are 16*16*16
#define NUM_ADDRESSABLE 208 // Total number of addressable channels are 13*16... within one crate


class Calibration {

private:

    struct Detector_t {
        XIA_address_t address;
        enum ADCSamplingFreq sfreq;
        enum DetectorType type;
        size_t telescope_number;
        size_t detector_number;

        double quad, gain, shift; // Energy calibration
        double cfd_shift; // Time calibration for CFD (between 0 and 1 ns)
        int timestamp_shift; // Time calibration for timestamp (whole numbers of ns)

        [[nodiscard]] inline double CalibrateEnergy(const unsigned short &channel) const
        {
            double ch = channel + drand48() - 0.5;
            return quad*ch*ch + gain*ch + shift;
        }

        [[nodiscard]] inline XIA::XIA_CFD_t CalibrateCFD(const unsigned short &channel) const
        {
            return XIA::XIA_CFD_Decode(sfreq, uint16_t(channel));
        }

    };

    std::array<Detector_t, NUM_ADDRESSABLE>  detectors;

    void Reset();

    inline Detector_t &GetDet(const unsigned short &crate,
                              const unsigned short &slot,
                              const unsigned short &channel)
    {
        return detectors[crate*256 + slot*16 + channel];
    }

    inline Detector_t &GetDet(const unsigned short &id){ return detectors[id]; }

    void Populate(); // To populate the internal array from singleton data.

public:

    Calibration();
    explicit Calibration(std::istream &json_data);

    void WriteOut(); // Write out all the parameters read to the terminal

    inline bool keep(const XIA_base_t *raw)
    {
        auto type = detectors[raw->index()].type;
        switch ( type ){
            case labr : return true;
            case deDet : return true;
            case eDet : return true;
            case ppac: return true;
            default: return false;
        }
    }

    Entry_t operator()(const XIA_base_t *raw)
    {
        Detector_t *detector = &detectors[raw->index()];
        int64_t timestamp = raw->timestamp() * (( detector->sfreq == f250MHz ) ? 8 : 10);

        double energy = detector->CalibrateEnergy(raw->eventEnergy);
        auto [cfdcorr, cfdres] = detector->CalibrateCFD(raw->cfd_result);

        timestamp += detector->timestamp_shift;
        cfdcorr += detector->cfd_shift;

        return {
            detector->type,
            static_cast<unsigned short>(detector->detector_number),
            static_cast<unsigned short>(raw->eventEnergy),
            static_cast<unsigned short>(raw->cfd_result),
            timestamp,
            raw->finishCode,
            energy,
            cfdcorr,
            cfdres
        };
    }
};


#endif // CALIBRATION_H
