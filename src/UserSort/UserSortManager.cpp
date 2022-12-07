//
// Created by Vetle Wegner Ingeberg on 20/10/2022.
//


#include "UserSortManager.h"
#include "UserSort.h"
#include "event.h"

UserSortManager::UserSortManager(ThreadSafeHistograms &hist, const char *libname)
        : library( libname )
        , object( reinterpret_cast<UserSort *>(library.GetObject(&hist, "NewUserSort")) )
{
}

UserSortManager::~UserSortManager()
{
    delete object;
}

void UserSortManager::FillEvent(const Triggered_event &event)
{
    if ( object )
        object->FillEvent(event);
}

void UserSortManager::Flush()
{
    if ( object )
        object->Flush();
}