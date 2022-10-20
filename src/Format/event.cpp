//
// Created by Vetle Wegner Ingeberg on 24/06/2022.
//

#include "event.h"

#include <algorithm>

Triggered_event::Triggered_event(const Triggered_event &event)
    : entries( event.entries )
    , trigger( event.trigger )
    , de_by_ring( NUM_SI_DE_TEL, {nullptr, nullptr} )
{
    index();
}

Triggered_event::Triggered_event(const std::vector<Entry_t> &_entries, const Entry_t &_trigger)
    : entries( _entries )
    , trigger( _trigger )
    , de_by_ring( NUM_SI_DE_TEL, {nullptr, nullptr} )
{
    //
    index();
}

Triggered_event::Triggered_event(std::vector<Entry_t> &&_entries, const Entry_t &_trigger)
    : entries( std::move(_entries) )
    , trigger( _trigger )
    , de_by_ring( NUM_SI_DE_TEL, {nullptr, nullptr} )
{
    //
    index();
}


void Triggered_event::index()
{
    // First step, sort the entries
    std::sort(entries.begin(), entries.end(), [](const auto &lhs, const auto &rhs){
        return lhs.type < rhs.type;
    });

    // Now we will sort through the detector types and fill our mapping for fast lookup later.
    for ( auto &type : {DetectorType::labr, DetectorType::deDet, DetectorType::eDet, DetectorType::ppac} ){
        auto begin = std::find_if(entries.begin(), entries.end(), [&type](const auto &c){ return c.type == type; });
        auto end = std::find_if_not(begin, entries.end(), [&type](const auto &c){ return c.type == type; });
        std::sort(begin, end, [](const auto &lhs, const auto &rhs){
            return lhs.detectorID < rhs.detectorID;
        });
        type_bounds[type] = subvector(&(*begin), &(*(end)));
    }

    // Lastly we will divide the DE by the ring numbers.
    auto de_events = GetDetector(DetectorType::deDet);
    for ( size_t ring = 0 ; ring < NUM_SI_DE_TEL ; ++ring ){
        auto begin = std::find_if(de_events.begin(), de_events.end(), [&ring](const auto &r){
            return ( r.detectorID / NUM_SI_DE_TEL ) == ring;
        });
        auto end = std::find_if(begin, de_events.end(), [&ring](const auto &r){
            return ( r.detectorID / NUM_SI_DE_TEL ) > ring;
        });

        de_by_ring[ring] = {begin, end};
    }

    /*auto begin = std::find_if(entries.begin(), entries.end(), [](const auto &c){
        return c.type == deDet;
    });
    auto end = std::find_if_not(begin, entries.end(), [](const auto &c){
        return c.type == eDet;
    });

    std::sort(begin, (end == entries.end()) ? end : end + 1, [](const auto &lhs, const auto &rhs){
        return lhs.detectorID < rhs.detectorID;
    });

    // Last point is to sort the particle entries by the detector ID to make life easier later on.
    std::sort(type_bounds[deDet].begin(), type_bounds[deDet].end(), [](const auto &lhs, const auto &rhs){
        return lhs.detectorID < rhs.detectorID;
    });
    std::sort(type_bounds[eDet].begin(), type_bounds[eDet].end(), [](const auto &lhs, const auto &rhs){
        return lhs.detectorID < rhs.detectorID;
    });*/
}

subvector<Entry_t> Triggered_event::GetRing(const size_t &ringNo)
{
    return de_by_ring[ringNo];
    /*auto ring_events = GetDetector(DetectorType::deDet);
    if ( ring_events.size() == 0 )
        return {0, 0};

    // Find the first ring event with ring No
    auto begin = std::find_if(ring_events.begin(), ring_events.end(), [&ringNo](const auto &r){
        //auto dnum = r.detectorID / NUM_SI_DE_TEL;
        return ( ( r.detectorID / NUM_SI_DE_TEL ) == ringNo );
    });

    auto end = std::find_if_not(begin, ring_events.end(), [&ringNo](const auto &r){
        return ( (r.detectorID / NUM_SI_DE_TEL ) == ringNo );
    });

    //if ( end - begin == 0 )
    //    return {0, 0};

    return {begin, end};*/
}