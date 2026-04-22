//
// Created by Vetle Wegner Ingeberg on 20/04/2026.
//

#include <TFile.h>
#include <TNamed.h>
#include <TParameter.h>

#include <string>

#include <hwinfo/hwinfo.h>
#include <hwinfo/utils/unit.h>

#include "MetadataWriter.h"

bool ProcessedDataMetadata::exportData() const {
    // Total number of events read from file
    if ( processedData.empty() )
        return false;
    //auto events_found = processedData[std::string("XIAReader")];
    TParameter events_found("RawEntries", static_cast<int64_t>(processedData.at("XIAReader")));
    if (events_found.Write() == 0) return false;

    TParameter entriesDropped("DroppedEvents", static_cast<int64_t>(processedData.at("Buffer")) - static_cast<int64_t>(processedData.at("Calibrator")));
    if (entriesDropped.Write() == 0) return false;

    TParameter eventsFound("EventsFound",  static_cast<int64_t>(processedData.at("Trigger")));
    if (eventsFound.Write() == 0) return false;

    TParameter triggeredEventsFound("TriggeredEvents", static_cast<int64_t>(processedData.at("Sorter")));
    if (triggeredEventsFound.Write() == 0) return false;

    return true;
}

bool MetadataWriter::WriteString(const std::string &key, const std::string &value) {
    TNamed obj(key.c_str(), value.c_str());
    return (obj.Write() > 0);
}

template<typename T>
bool MetadataWriter::WriteNumeric(const std::string &key, const T &value) {
    TParameter param(key.c_str(), value);
    return (param.Write() > 0);
}

MetadataWriter::MetadataWriter(const char* fname, const char* folder_name, const char* options)
    : file(fname, options)
    , metaDir( file.GetDirectory(folder_name) )
{
    if ( !metaDir ) {
        metaDir = file.mkdir(folder_name);
    }
}

bool MetadataWriter::WriteSystemInfo() {
    auto cpus = hwinfo::getAllCPUs();
    WriteString("CPU.vendor", cpus[0].vendor());
    WriteString("CPU.model", cpus[0].modelName());
    WriteNumeric("CPU.physicalCores", cpus[0].numPhysicalCores());
    WriteNumeric("CPU.logicalCores", cpus[0].numLogicalCores());
    WriteNumeric("CPU.frequency", cpus[0].regularClockSpeed_MHz());

    hwinfo::Memory memory;
    WriteNumeric("Memory.totalSize", memory.total_Bytes());

    hwinfo::OS os;
    WriteString("OS.name", os.name());
    WriteString("OS.kernel", os.kernel());
    WriteString("OS.version", os.version());
    WriteString("OS.endian", os)
}

MetadataWriter::~MetadataWriter() {
    Write();
    file.Close();
}

bool MetadataWriter::Write() {
    metaDir->cd();
    if (!processedData.exportData()) return false;
    return (file.Write() > 0);
}