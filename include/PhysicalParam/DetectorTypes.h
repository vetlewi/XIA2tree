//
// Created by Vetle Wegner Ingeberg on 19/04/2023.
//

#ifndef DETECTORTYPES_H
#define DETECTORTYPES_H

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
