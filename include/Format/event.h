//
// Created by Vetle Wegner Ingeberg on 24/06/2022.
//

#ifndef XIA2TREE_EVENT_H
#define XIA2TREE_EVENT_H

#include <vector>
#include <map>
#include <entry.h>

#include <experimentsetup.h>

template<typename T>
class subvector
{
private:
    T *_begin;
    T *_end;

public:

    inline T *begin(){ return _begin; }
    inline T *end(){ return _end; }

    inline T *begin() const { return _begin; }
    inline T *end() const { return _end; }

    [[nodiscard]] inline size_t size() const { return _end - _begin; }

    inline T *operator[](const size_t &idx){ return _begin + idx; }
    inline T *operator[](const size_t &idx) const { return _begin + idx; }

    subvector() : _begin( nullptr ), _end( nullptr ){}
    subvector(T *begin, T *end) : _begin( begin ), _end( end ){}
};

class Triggered_event {
private:
    std::vector<Entry_t> entries;
    Entry_t trigger;
    std::map<DetectorType, subvector<Entry_t>> type_bounds;
    std::vector<subvector<Entry_t>> de_by_ring;

    void index(); // Should run each initialization, unless empty.

public:
    Triggered_event() = default;
    Triggered_event(const Triggered_event &event);
    explicit Triggered_event(const std::vector<Entry_t> &_entries, const Entry_t &trigger);
    explicit Triggered_event(std::vector<Entry_t> &&_entries, const Entry_t &trigger);

    inline std::vector<Entry_t> GetEntries() const { return entries; }
    inline subvector<Entry_t> GetDetector(const DetectorType &type) const { return type_bounds.at(type); }
    inline const Entry_t *GetTrigger() const { return ( trigger.type == unused && trigger.detectorID == uint16_t(-1) ) ? nullptr : &trigger; }

    subvector<Entry_t> GetRing(const size_t &ringNo);
};

#endif //XIA2TREE_EVENT_H
