//
// Created by Vetle Wegner Ingeberg on 20/04/2026.
//

#include <TFile.h>
#include <TNamed.h>
#include <TParameter.h>

#include "MetadataWriter.h"

void MetadataWriter::WriteString(const std::string &key, const std::string &value) {
    TNamed obj(key.c_str(), value.c_str());
    obj.Write();
}

template<typename T>
void MetadataWriter::WriteNumeric(const std::string &key, const T &value) {
    TParameter param(key.c_str(), value);
    param.Write();
}

bool MetadataWriter::Write(const char* fname, const char* folder_name, const char* options) {
    TFile file(fname, options);
    auto metaDir = file.GetDirectory(folder_name);
    if ( !metaDir ) {
        metaDir = file.mkdir(folder_name);
    }
    metaDir->cd();

    // Now we can start writing to file...


    file.Write();
    file.Close();
    return true;
}