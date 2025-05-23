//
// Created by Vetle Wegner Ingeberg on 19/04/2023.
//

#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <iosfwd>
#include <vector>
#include <PhysicalParam/DetectorTypes.h>
#include <PhysicalParam/XIA_CFD.h>
#include <Format/xiaformat.h>
#include <Format/entry.h>

class ParticleRange;

namespace YAML {
    class Node;
}

namespace OCL {

    class UserConfiguration {
    private:
        const YAML::Node &userConfig;
        const ParticleRange &range;

    public:
        UserConfiguration(const YAML::Node &userConfig, const ParticleRange &range);
        static UserConfiguration FromFile(const char *file, const ParticleRange &range);
        static UserConfiguration FromFile(std::istream &, const ParticleRange &);

        inline const YAML::Node &GetConfig() const { return userConfig; }
        inline const ParticleRange &GetRange() const { return range; }
    };

    class ConfigManager {
    private:

        struct DetectorInfo_t {
            size_t address;
            enum XIA::ADCSamplingFreq sfreq;
            enum DetectorType type;
            size_t detectorID;

            double quad, gain, shift;
            double cfd_shift;
            long long timestamp_shift;

            [[nodiscard]] inline double CalibrateEnergy(const unsigned short &channel) const
            {
                double ch = channel + (drand48() - 0.5);
                return quad*ch*ch + gain*ch + shift;
            }

            [[nodiscard]] inline XIA::XIA_CFD_t CalibrateCFD(const unsigned short &channel) const
            {
                return XIA::XIA_CFD_Decode(sfreq, uint16_t(channel));
            }
        };

        std::vector<DetectorInfo_t> dinfo;

    public:
        explicit ConfigManager(const YAML::Node &setup);
        static ConfigManager FromFile(const char *file);
        static ConfigManager FromFile(std::istream &);

        inline bool keep(const XIA_base_t *xia){
            auto type = dinfo.at(xia->index()).type;
            switch ( type ) {
                case labr:
                case deDet:
                case eDet:
                case ppac:
                    return true;
                default:
                    return false;
            }
        }

        inline Entry_t operator()(const XIA_base_t *raw) const
        {
            auto *detector = &dinfo.at(raw->index());

            int64_t timestamp = raw->timestamp() * (( detector->sfreq == XIA::f250MHz ) ? 8 : 10);

            double energy = detector->CalibrateEnergy(raw->eventEnergy);
            auto [cfdcorr, cfdres] = detector->CalibrateCFD(raw->cfd_result);

            timestamp += detector->timestamp_shift;
            cfdcorr += detector->cfd_shift;

            return {
                    detector->type,
                    static_cast<unsigned short>(detector->detectorID),
                    static_cast<unsigned short>(raw->eventEnergy),
                    static_cast<unsigned short>(raw->cfd_result),
                    timestamp,
                    raw->finishCode,
                    energy,
                    cfdcorr,
                    cfdres,
                    getQDC(raw),
                    getTrace(raw)
            };
        }

    };
}

#endif // CONFIGMANAGER_H
