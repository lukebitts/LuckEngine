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
            bool _running;
            LuckWindow();
        public:
            s32 width;
            s32 height;
            void updateWindowSize();
            void setWindowCaption(string caption);
            bool isRunning();
            static void keyCallback(s32 key, s32 action);
            static void mouseMoveCallback(s32 x, s32 y);
            static void mouseClickCallback(s32 button, s32 action);
            static void mouseWheelCallback(s32 position);
            static LuckWindow* getInstance()
            {
                if(!_instance) _instance = new LuckWindow();
                return _instance;
            }
    };
}

luck::LuckWindow* createLuckWindow(luck::u16 width, luck::u16 height, luck::u16 redbits = 0, luck::u16 greenbits = 0, luck::u16 bluebits = 0,
               luck::u16 alphabits = 0, luck::u16 depthbits = 0, luck::u16 stencilbits = 0);
#endif
