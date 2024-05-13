//
// Created by Vetle Wegner Ingeberg on 20/10/2022.
//

#ifndef DYNAMICLIBRARY_H
#define DYNAMICLIBRARY_H

class DynamicLibrary {
private:
    void *_handle;
public:
    DynamicLibrary() = default;
    explicit DynamicLibrary(const char *libname);
    ~DynamicLibrary();

    void *GetObject(void *hist_manager, const char *config_file, const char *constructor);

};

#endif // DYNAMICLIBRARY_H
