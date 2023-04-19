//
// Created by Vetle Wegner Ingeberg on 04/04/2023.
//

#include "TTreeManager.h"

using namespace Task;

std::string BranchName(const char *name, const char *varname)
{
    return std::string(name) + "_" + std::string(varname);
}

std::string LeafList(const char *name, const char *varname, const bool &mult, const char &type)
{
    std::string result;
    if ( mult ) {
        result.reserve(snprintf(nullptr, 0, "%s%s[%s%s]/%c", name, varname, name, "Mult", type) + 1);
        snprintf(result.data(), result.capacity(), "%s%s[%s%s]/%c", name, varname, name, "Mult", type);
    } else {
        result.reserve(snprintf(nullptr, 0, "%s%s/%c", name, varname, type) + 1);
        snprintf(result.data(), result.capacity(), "%s%s/%c", name, varname, type);
    }
    return result;
}

template<typename T>
TBranch *make_leaf(TTree *tree, T *addr, const char *base_name, const char *varname, const bool &mult, const char &type)
{
    return tree->Branch(BranchName(base_name, varname).c_str(),
                        addr,
                        LeafList(base_name, varname, mult, type).c_str());
}

details::TriggerEntry::TriggerEntry(TTree &tree)
{
    make_leaf(&tree, &ID, "Trigger", "ID", false, 's');
    make_leaf(&tree, &finishflag, "Trigger", "FinishFlag", false, 'O');
    make_leaf(&tree, &adcvalue, "Trigger", "ADCValue", false, 's');
    make_leaf(&tree, &timestamp, "Trigger", "timestamp", false, 'L');
    make_leaf(&tree, &energy, "Trigger", "energy", false, 'D');
    make_leaf(&tree, &cfdfail, "Trigger", "CFDfail", false, 'O');
    make_leaf(&tree, &cfdcorr, "Trigger", "CFDcorr", false, 'D');
    make_leaf(&tree, &idx, "Trigger", "IDX", false, 's');
}

details::DetectorEntries::DetectorEntries(TTree &tree, const char *base_name)
    : mult( 0 )
{
    make_leaf(&tree, &mult, base_name, "Mult", false, 's');
    make_leaf(&tree, ID, base_name, "ID", true, 's');
    make_leaf(&tree, finishflag, base_name, "FinishFlag", true, 'O');
    make_leaf(&tree, adcvalue, base_name, "ADCValue", true, 's');
    make_leaf(&tree, timestamp, base_name, "timestamp", true, 'L');
    make_leaf(&tree, energy, base_name, "energy", true, 'D');
    make_leaf(&tree, cfdfail, base_name, "CFDfail", true, 'O');
    make_leaf(&tree, cfdcorr, base_name, "CFDcorr", true, 'D');
}