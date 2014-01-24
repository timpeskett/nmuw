#ifndef FAKEINPUT_H
#define FAKEINPUT_H

#include "common.h"
#include <string.h>

#ifdef __linux
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#endif

Bool isTestsAvailable( Display *disp );

void hidePointer( Display *disp );
void setPointerPos( Display *disp, int x, int y );
void movePointer( Display *disp, int dx, int dy );
void pointerPress( Display *disp, int button );
void pointerRelease( Display *disp, int button );

void buttonPress( Display *disp, int button );
void buttonRelease( Display *disp, int button );


#endif
