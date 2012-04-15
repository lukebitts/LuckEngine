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

/*luck::s32 createLuckWindow(luck::u16 width, luck::u16 height, luck::u16 redbits = 0, luck::u16 greenbits = 0, luck::u16 bluebits = 0,
               luck::u16 alphabits = 0, luck::u16 depthbits = 0, luck::u16 stencilbits = 0);*/
#endif
