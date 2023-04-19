//
// Created by Vetle Wegner Ingeberg on 06/04/2022.
//

#include "Tools/enumerate.h"

#include "PhysicalParam/Calibration.h"
#include "PhysicalParam/Parameters.h"

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

/*
 * We expect the following json format
{
"hardware": {
    "crate": {
        "0": [{
                "energy": {
                "shift": value
                }
             ]
        }
    }
}


 */

static Parameters calParam;

//! Parameters for energy calibration of the LaBr detectors
static Parameter quad_labr(calParam, "quad_labr", NUM_LABR_DETECTORS, 0);
static Parameter gain_labr(calParam, "gain_labr", NUM_LABR_DETECTORS, 1);
static Parameter shift_labr(calParam, "shift_labr", NUM_LABR_DETECTORS, 0);

//! Parameters for energy calibration of the Si detectors
static Parameter quad_de(calParam, "quad_de", NUM_SI_DE_DET, 0);
static Parameter gain_de(calParam, "gain_de", NUM_SI_DE_DET, 1);
static Parameter shift_de(calParam, "shift_de", NUM_SI_DE_DET, 0);

static Parameter quad_e(calParam, "quad_e", NUM_SI_E_DET, 0);
static Parameter gain_e(calParam, "gain_e", NUM_SI_E_DET, 1);
static Parameter shift_e(calParam, "shift_e", NUM_SI_E_DET, 0);

static Parameter t0_labr(calParam, "t0_labr", NUM_LABR_DETECTORS, 0);
static Parameter t0_de(calParam, "t0_de", NUM_SI_DE_DET, 0);
static Parameter t0_e(calParam, "t0_e", NUM_SI_E_DET, 0);
static Parameter t0_ppac(calParam, "t0_ppac", NUM_PPAC, 0);

Parameter::param_t GetQuad(const DetectorInfo_t *dinfo)
{
    switch ( dinfo->type ){
        case labr :
            return quad_labr[dinfo->detectorNum];
        case deDet :
            return quad_de[dinfo->detectorNum];
        case eDet :
            return quad_e[dinfo->detectorNum];
        default:
            return 1;
    }
}

Parameter::param_t GetGain(const DetectorInfo_t *dinfo)
{
    switch ( dinfo->type ){
        case labr :
            return gain_labr[dinfo->detectorNum];
        case deDet :
            return gain_de[dinfo->detectorNum + NUM_SI_DE_TEL * dinfo->telNum];
        case eDet :
            return gain_e[dinfo->detectorNum];
        default:
            return 1;
    }
}

Parameter::param_t GetShift(const DetectorInfo_t *dinfo)
{
    switch ( dinfo->type ){
        case labr :
            return shift_labr[dinfo->detectorNum];
        case deDet :
            return shift_de[dinfo->detectorNum + NUM_SI_DE_TEL * dinfo->telNum];
        case eDet :
            return shift_e[dinfo->detectorNum];
        default:
            return 0;
    }
}

Parameter::param_t GetTime(const DetectorInfo_t *dinfo)
{
    switch ( dinfo->type ){
        case labr :
            return t0_labr[dinfo->detectorNum];
        case deDet :
            return t0_de[dinfo->detectorNum + NUM_SI_DE_TEL * dinfo->telNum]; // Stupid hack... need a better solution
        case eDet :
            return t0_e[dinfo->detectorNum];
        case ppac :
            return t0_ppac[dinfo->detectorNum];
        default:
            return 0;
    }
}

bool NextLine(std::istream &in, std::string &outline, int &lineno)
{
    outline = "";
    std::string line;
    while ( std::getline(in, line) ){
        lineno++;
        size_t ls = line.size();
        if ( ls == 0 ){
            break;
        } else if ( line[ls - 1] != '\\' ){
            outline += line;
            break;
        } else {
            outline += line.substr(0, ls-1);
        }
    }
    return in || !line.empty();
}

bool SetCalibration(const char *calfile)
{
    // Open file
    std::ifstream inCal(calfile);
    std::string currentLine;
    int lineno = 0; // Keep track of line read. To make it easier to debug :)

    // Get line by line!
    while ( NextLine(inCal, currentLine, lineno) ){
        if ( currentLine[0] == '#' ) // skip lines begining with #
            continue;
        std::istringstream icmd(currentLine);
        if ( !calParam.SetAll(icmd) ){
            std::cerr << "Error extracting calibration from line ";
            std::cerr << lineno << " in '" << calfile << "': ";
            std::cerr << currentLine << std::endl;
            return false;
        }
    }
    // Make sure we have time calibration on the correct format.
    return true;
}

bool SetCalibration(std::istream &inCal)
{
    // Open file
    std::string currentLine;
    int lineno = 0; // Keep track of line read. To make it easier to debug :)

    // Get line by line!
    while ( NextLine(inCal, currentLine, lineno) ){
        if ( currentLine.empty() )
            continue;
        auto use_line = currentLine.substr(currentLine.find_first_not_of(' '), currentLine.find_last_of('#'));
        if ( use_line[0] == '#' )
            continue;
        std::istringstream icmd(use_line);
        if ( !calParam.SetAll(icmd) ){
            std::cerr << "Error extracting calibration from line ";
            std::cerr << lineno << ": " << currentLine << std::endl;
            return false;
        }
    }
    // Make sure we have time calibration on the correct format.
    return true;
}


void Calibration::Reset()
{
    // This is when we do not have anything...
    // We will then populate everything with the default. Ie. zeros...
    for ( auto&& [index, entry] : enumerate(detectors) ){
        auto address = reinterpret_cast<XIA_address_t *>(&index);
        entry = {*address, f000MHz, invalid, 0, 0,
                 0, 0, 0, 0};
    }
}

void Calibration::Populate()
{
    const DetectorInfo_t *dinfo;

    for ( auto&& [index, entry] : enumerate(detectors) ){
        dinfo = GetDetectorPtr(index);

        entry = {
                *reinterpret_cast<XIA_address_t *>(&index),
                dinfo->sfreq,
                dinfo->type,
                dinfo->telNum,
                dinfo->detectorNum,
                GetQuad(dinfo),
                GetGain(dinfo),
                GetShift(dinfo),
                GetTime(dinfo) - int(GetTime(dinfo)),
                int(GetTime(dinfo))
        };
        if ( dinfo->type == deDet ) { // Stupid temporary hack!!
            entry.detector_number += NUM_SI_DE_TEL * entry.telescope_number;
        }
    }
}

Calibration::Calibration()
{
    Reset();
    Populate();
    WriteOut();
}



Calibration::Calibration(std::istream &in)
{
    Reset();

    // Now we will extract all the important information about each crate, module, etc.
    // Currently I will use the old routines to do this. In the future will move to either yaml, toml or json.
    if (!SetCalibration(in)){
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__)+": Parsing error");
    }

    Populate();
}

void Calibration::WriteOut()
{

}