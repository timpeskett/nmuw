#ifndef SCREEN_H
#define SCREEN_H

#include "AppData.h"
#ifdef __linux
#include <X11/Xlib.h>
#endif

int getScreenWidth( AppData *ad );

int getScreenHeight( AppData *ad );


#endif
