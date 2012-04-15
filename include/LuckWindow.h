#ifndef LUCKWINDOW_H
#define LUCKWINDOW_H
#include "luck.h"
#include "EventDispatcher.h"
namespace luck
{
    class LuckWindow : public event::EventDispatcher
    {
        private:
            static LuckWindow* _instance;
            LuckWindow();
        public:
            static LuckWindow* getInstance()
            {
                if(!_instance) _instance = new LuckWindow();
                return _instance;
            }
    };
}

luck::LuckWindow* createLuckWindow(luck::u16 width, luck::u16 height, luck::u16 redbits, luck::u16 greenbits, luck::u16 bluebits,
               luck::u16 alphabits, luck::u16 depthbits, luck::u16 stencilbits);
#endif
