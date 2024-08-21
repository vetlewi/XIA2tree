//
// Created by Vetle Wegner Ingeberg on 20/10/2022.
//


#include "UserSortManager.h"
#include "UserSort.h"
#include "event.h"
#include "ConfigManager.h"

UserSortManager::UserSortManager(ThreadSafeHistograms &hist, const OCL::UserConfiguration &config, const char *libname)
        : library( libname )
        , object( reinterpret_cast<UserSort *>(library.GetObject(&hist, &config, "NewUserSort")) )
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