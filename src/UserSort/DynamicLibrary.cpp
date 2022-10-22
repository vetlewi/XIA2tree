//
// Created by Vetle Wegner Ingeberg on 20/10/2022.
//


#include "DynamicLibrary.h"

#include <dlfcn.h>
#include <iostream>

DynamicLibrary::DynamicLibrary(const char *libname)
        : _handle( ( libname ) ? dlopen(libname, RTLD_LAZY) : nullptr )
{
    if ( !_handle && libname ){ // Only error if the library name is not a nullptr.
        std::cerr << "Unable to load '" << libname << ". Got error '";
        std::cerr << dlerror() << "'." << std::endl;
    }
}

DynamicLibrary::~DynamicLibrary()
{
    if ( _handle )
        dlclose(_handle);
    _handle = nullptr;
}

void *DynamicLibrary::GetObject(void *ptr, const char *constructor)
{
    if ( !_handle ){
        return nullptr; // It is completely fine if we don't have anything. Skip if not present.
    }
    using Func = void*(*)(void *);
    auto sym = reinterpret_cast<Func>(dlsym(_handle, constructor));
    if ( !sym ){
        std::cerr << "Could not load NewUserSort. Got error '" << dlerror() << "'" << std::endl;
        return nullptr;
    }
    return sym(ptr);
}