//
// Created by Vetle Wegner Ingeberg on 04/04/2023.
//

#include "TTreeManager.h"

using namespace Task;

std::string BranchName(const char *name, const char *varname)
{
    return std::string(name) + "_" + std::string(varname);
}

std::string BranchList(const char *name, const char *varname, const bool &mult, const char &type)
{
    std::string result;
    if ( mult ) {
        result.reserve(snprintf(nullptr, 0, "%s[%s]/%c",
                                BranchName(name, varname).c_str(), BranchName(name, "Mult").c_str(), type) + 1);
        snprintf(result.data(), result.capacity(), "%s[%s]/%c",
                 BranchName(name, varname).c_str(), BranchName(name, "Mult").c_str(), type);
    } else {
        result.reserve(snprintf(nullptr, 0, "%s/%c", BranchName(name, varname).c_str(), type) + 1);
        snprintf(result.data(), result.capacity(), "%s/%c", BranchName(name, varname).c_str(), type);
    }
    return result;
}

std::string BranchListArray(const char *name, const char *varname, const bool &mult, const char &type, int array_size)
{
    std::string result;
    if ( mult ) {
        result.reserve(snprintf(nullptr, 0, "%s[%s][%i]/%c",
                                BranchName(name, varname).c_str(), BranchName(name, "Mult").c_str(), array_size, type) + 1);
        snprintf(result.data(), result.capacity(), "%s[%s][%i]/%c",
                 BranchName(name, varname).c_str(), BranchName(name, "Mult").c_str(), array_size, type);
    } else {
        result.reserve(snprintf(nullptr, 0, "%s[%i]/%c", BranchName(name, varname).c_str(), array_size, type) + 1);
        snprintf(result.data(), result.capacity(), "%s[%i]/%c", BranchName(name, varname).c_str(), array_size, type);
    }
    return result;
}

template<typename T>
TBranch *make_branch(TTree *tree, T *addr, const char *base_name, const char *varname, const bool &mult, const char &type)
{
    return tree->Branch(BranchName(base_name, varname).c_str(),
                        addr,
                        BranchList(base_name, varname, mult, type).c_str());
}

template<typename T>
TBranch *make_branch_array(TTree *tree, T *addr, const char *base_name, const char *varname, const bool &mult, const char &type, int array_size)
{
    return tree->Branch(BranchName(base_name, varname).c_str(),
                        addr,
                        BranchListArray(base_name, varname, mult, type, array_size).c_str());
}

template<typename T>
TBranch *make_branch_STL(TTree *tree, T *addr, const char *base_name, const char *varname) {
    return tree->Branch(BranchName(base_name, varname).c_str(), addr);
}

details::TriggerEntry::TriggerEntry(TTree &tree, const bool& traces)
    : ID( ), finishflag( ), adcvalue( ), timestamp( ), energy( ), cfdfail( ), cfdcorr( ), idx( ), qdc( )
{
    make_branch(&tree, &ID, "Trigger", "ID", false, 's');
    make_branch(&tree, &finishflag, "Trigger", "FinishFlag", false, 'O');
    make_branch(&tree, &adcvalue, "Trigger", "ADCValue", false, 'i');
    make_branch(&tree, &timestamp, "Trigger", "timestamp", false, 'L');
    make_branch(&tree, &energy, "Trigger", "energy", false, 'D');
    make_branch(&tree, &cfdfail, "Trigger", "CFDfail", false, 'O');
    make_branch(&tree, &cfdcorr, "Trigger", "CFDcorr", false, 'D');
    make_branch(&tree, &idx, "Trigger", "IDX", false, 's');
    if ( traces ) {
        make_branch_array(&tree, qdc.data(), "Trigger", "QDC", false, 'i', QDC_SIZE);
        make_branch_STL(&tree, &trace,"Trigger", "Trace");
    }
}

details::DetectorEntries::DetectorEntries(TTree &tree, const char *base_name, const bool& traces)
    : mult( 0 ), ID( ), finishflag( ), adcvalue( ), timestamp( ), energy( ), cfdfail( ), cfdcorr( ), qdc( )
{
    make_branch(&tree, &mult, base_name, "Mult", false, 'I');
    make_branch(&tree, ID, base_name, "ID", true, 's');
    make_branch(&tree, finishflag, base_name, "FinishFlag", true, 'O');
    make_branch(&tree, adcvalue, base_name, "ADCValue", true, 'i');
    make_branch(&tree, timestamp, base_name, "timestamp", true, 'L');
    make_branch(&tree, energy, base_name, "energy", true, 'D');
    make_branch(&tree, cfdfail, base_name, "CFDfail", true, 'O');
    make_branch(&tree, cfdcorr, base_name, "CFDcorr", true, 'D');
    make_branch_array(&tree, qdc, base_name, "QDC", true, 'i', QDC_SIZE);
    if ( traces ) {
        make_branch(&tree, &trace_entries, base_name, "Trace_entries", true, 'I');
        make_branch_array(&tree, &trace, base_name, "Trace", true, 's', TRACE_MAX_LENGTH);
    }
}