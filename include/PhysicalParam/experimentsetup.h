#ifndef EXPERIMENTSETUP_H
#define EXPERIMENTSETUP_H

// Currently the sorting rutine will only support dE-E silicon telescopes.
// This may change in the future if needed... I think...

#define NUM_LABR_DETECTORS 30   //!< Number of LaBr detectors
#define NUM_SI_DE_DET 64        //!< Number of Si dE sections
#define NUM_SI_DE_TEL 8         //!< Number of si dE sections per trapezoidal
#define NUM_SI_E_DET 8          //!< Number of E Si rings
#define NUM_SI_E_GUARD 8        //!< Number of E guard rings
#define NUM_PPAC       4

#define TOTAL_NUMBER_OF_MODULES 7   //!< Number of modules

#define TOTAL_NUMBER_OF_ADDRESSES 175   //! Total number of address that needs to be defined

#include <cstdint>

/*enum DetectorType {
    invalid,    //!< Invalid address
    labr,       //!< Is a labr detector
    deDet,      //!< Is a Delta-E segment
    eDet,       //!< Is a E detector
    eGuard,     //!< Is a E guard ring
    ppac,       //!< Is a PPAC
    rfchan,     //!< Is a RF channel
    any,        //!< Any detector
    unused      //!< Is a unused XIA channel
};*/

/*enum ADCSamplingFreq {
    f100MHz,    //!< 100 MHz sampling frequency
    f250MHz,    //!< 250 MHz sampling frequency
    f500MHz,    //!< 500 MHz sampling frequency
    f000MHz     //!< If invalid address
};

struct DetectorInfo_ {
    uint16_t address;           //!< ADC address of the detector
    enum ADCSamplingFreq sfreq; //!< ADC sampling frequency
    enum DetectorType type;     //!< Type of detector
    uint16_t detectorNum;            //!< 'Linear' number of the detector
    uint16_t telNum;                 //!< Telescope number (ie. E back detector for the dE front detector)
};

typedef struct DetectorInfo_ DetectorInfo_t;

 */

//! Get detector method
/*! \return Detector structure containing information about the
 *  detector at address.
 */
//DetectorInfo_t GetDetector(uint16_t address   /*!< Address of the detector to get */);

//! Get Detector ptr method
/*!
 * \return pointer to the detector entry
 * Potentially unsafe as it isn't bounds checked.
 */
//const DetectorInfo_t *GetDetectorPtr(uint16_t address);

//! Get sampling frequency
/*! \return The XIA module sampling frequency
 */
//enum ADCSamplingFreq GetSamplingFrequency(uint16_t address    /*!< ADC address    */);



#endif // EXPERIMENTSETUP_H
