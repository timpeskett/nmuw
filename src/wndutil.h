#ifndef WNDUTIL_H
#define WNDUTIL_H

#include "common.h"
#include "Wnd.h"
#include <string.h>
#include <X11/Xlib.h>

void drawCenteredText( Display *disp, Wnd *inWnd, const char *inString, int inLen );

void percentToAbsoluteDim( Wnd *subWnd, int parentw, int parenth );

int GetTextHeight( Display *disp, Wnd *inWnd, const char *inString );

/* Draws text at specified point in specified window. If text requires more space than provided by specified bounding box (bounding box is x, y, x + maxWidth, y + maxHeight ) then the text will not be drawn. If invert is true then the text will be drawn with background and foreground colours swapped (i.e will be highlighted) */
void DrawText( Display *disp, Wnd *inWnd, const char *inString, int x, int y, int maxWidth, int maxHeight, boolean invert );

/* Returns the closest value to desired available. desired should be in format 0xRRGGBB */
unsigned int getPixelValue( Display *disp, unsigned int desired );

GC CreateDefaultGC( Display *disp, Wnd *inWnd, unsigned int fore, unsigned int back );


#endif

