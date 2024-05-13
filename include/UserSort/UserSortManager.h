//
// Created by Vetle Wegner Ingeberg on 20/10/2022.
//

#ifndef XIA2TREE_USERSORTMANAGER_H
#define XIA2TREE_USERSORTMANAGER_H

#include <UserSort/DynamicLibrary.h>

class ThreadSafeHistograms;
class UserSort;
class Triggered_event;

namespace OCL {
    class ConfigManager;
}

class UserSortManager
{
private:
    DynamicLibrary library;
    UserSort *object;

public:
    UserSortManager(ThreadSafeHistograms &hist, const char *libname = nullptr, const char *configfile = nullptr);
    ~UserSortManager();
    void FillEvent(const Triggered_event &event);
    void Flush();
};

#endif //XIA2TREE_USERSORTMANAGER_H
