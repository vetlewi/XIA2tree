//
// Created by Vetle Wegner Ingeberg on 19/04/2023.
//

#ifndef DETECTORTYPES_H
#define DETECTORTYPES_H

#define NUM_LABR_DETECTORS 30   //!< Number of LaBr detectors
#define NUM_SI_DE_DET 64        //!< Number of Si dE sections
#define NUM_SI_DE_TEL 8         //!< Number of si dE sections per trapezoidal
#define NUM_SI_E_DET 8          //!< Number of E Si rings
#define NUM_SI_E_GUARD 8        //!< Number of E guard rings
#define NUM_PPAC       4        //!< Number of PPAC detectors

enum DetectorType {
    invalid,    //!< Invalid address
    labr,       //!< Is a labr detector
    deDet,      //!< Is a Delta-E segment
    eDet,       //!< Is a E detector
    eGuard,     //!< Is a E guard ring
    ppac,       //!< Is a PPAC
    rfchan,     //!< Is a RF channel
    any,        //!< Any detector
    unused      //!< Is a unused XIA channel
};


#endif // DETECTORTYPES_H
