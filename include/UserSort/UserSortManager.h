//
// Created by Vetle Wegner Ingeberg on 20/10/2022.
//

#ifndef XIA2TREE_USERSORTMANAGER_H
#define XIA2TREE_USERSORTMANAGER_H

#include <DynamicLibrary.h>
#include <UserSort.h>

class ThreadSafeHistograms;


class UserSortManager
{
private:
    DynamicLibrary library;
    UserSort *object;

public:
    UserSortManager(ThreadSafeHistograms &hist, const char *libname = nullptr);
    ~UserSortManager();
    void FillEvent(const Triggered_event &event);
};

#endif //XIA2TREE_USERSORTMANAGER_H
