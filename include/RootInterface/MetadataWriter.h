//
// Created by Vetle Wegner Ingeberg on 20/04/2026.
//

#ifndef XIA2TREE_METADATAWRITER_H
#define XIA2TREE_METADATAWRITER_H

#include <string>

class MetadataWriter {
private:
    static void WriteString(const std::string& key, const std::string& value);

    template <typename T>
    static void WriteNumeric(const std::string& key, const T& value);


public:


    static bool Write(const char* fname, const char* folder_name="Metadata", const char* options="UPDATE");

};

#endif //XIA2TREE_METADATAWRITER_H
