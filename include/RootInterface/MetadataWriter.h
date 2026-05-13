//
// Created by Vetle Wegner Ingeberg on 20/04/2026.
//

#ifndef XIA2TREE_METADATAWRITER_H
#define XIA2TREE_METADATAWRITER_H

#include <string>

#include <TFile.h>
#include <TDirectory.h>

struct ProcessedDataMetadata {
    std::map<std::string, size_t> processedData;
    bool exportData() const;
};

class MetadataWriter {
private:
    TFile file;
    TDirectory *metaDir;

    ProcessedDataMetadata processedData;

    bool WriteString(const std::string& key, const std::string& value);

    template <typename T>
    bool WriteNumeric(const std::string& key, const T& value);

    bool WriteSystemInfo();

public:
    MetadataWriter(const char* fname, const char* folder_name="Metadata", const char* options="UPDATE");
    ~MetadataWriter();

    void AddProcessedMetadata(const std::map<std::string, size_t>& map){ processedData = {map};}
    bool Write();

};

#endif //XIA2TREE_METADATAWRITER_H
