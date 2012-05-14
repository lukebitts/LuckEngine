#ifndef LOADPROGRESSEVENT_H
#define LOADPROGRESSEVENT_H
#include "event.h"
namespace luck { namespace event
{
    struct LoadProgressEvent : Event
    {
        string file;
        u32 loaded;
        u32 total;
        f32 percent;
        LoadProgressEvent(string file, u32 loaded, u32 total) : file(file), loaded(loaded), total(total)
        {
            percent = (f32)loaded / total * 100;
        }
    };
}}
#endif
