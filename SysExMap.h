// Maps plugin parameters to the <page> byte of a QuadraVerb SysEx parameter packet.
//
// The QuadraVerb reference manual does not publish the SysEx page numbering, so this table is EMPTY on purpose: while a
// parameter has no entry here, "mirror panel edits" sends nothing and incoming packets for unknown pages are ignored.
// Use the SysEx panel's test sender (func / page / value) to probe the real unit, then fill in the table, e.g.
//     { "rtype", 0x10 }, { "rdecay", 0x11 }, ...
#pragma once
#include <cstring>

struct SysExPageEntry { const char* id; int page; };
static const SysExPageEntry kSysExPages[] = {
    { nullptr, 0 }   // terminator: add entries above this line
};
static inline int sysexPageFor (const char* id)
{
    for (const auto* e = kSysExPages; e->id != nullptr; ++e) if (! std::strcmp (e->id, id)) return e->page;
    return -1;
}
static inline const char* sysexIdForPage (int page)
{
    for (const auto* e = kSysExPages; e->id != nullptr; ++e) if (e->page == page) return e->id;
    return nullptr;
}
