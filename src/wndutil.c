#include "wndutil.h"


void drawCenteredText( Display *disp, Wnd *inWnd, const char *inString, int inLen )
{
	XFontStruct *gcFont;
	int width, ascent, descent, direction;
	XCharStruct overall_return;
	int midx, midy;

	gcFont = XQueryFont( disp, XGContextFromGC( inWnd->wndGC ) );
	width = XTextWidth( gcFont, inString, inLen );
	XTextExtents( gcFont, inString, inLen, &direction, &ascent, &descent, &overall_return );
	XFreeFontInfo( NULL, gcFont, 0 );

	midx = inWnd->data.width / 2;
	midy = inWnd->data.height / 2;
	XDrawString( disp, inWnd->wndHandle, inWnd->wndGC, midx - width / 2, midy + descent, inString, inLen );
}


/* Assumes that percentage data is already loaded into outWnd->data */
void percentToAbsoluteDim( Wnd *subWnd, int parentw, int parenth )
{
	subWnd->data.x = (int)( subWnd->data.x / 100.0 * parentw );
	subWnd->data.y = (int)( subWnd->data.y / 100.0 * parenth );
	subWnd->data.width = (int)( subWnd->data.width / 100.0 * parentw );
	subWnd->data.height = (int)( subWnd->data.height / 100.0 * parenth );
}

int GetTextHeight( Display *disp, Wnd *inWnd, const char *inString )
{
	XFontStruct *gcFont;
	int ascent, descent, direction;
	XCharStruct overall_return;

	int textLen;

	textLen = strlen( inString );

	gcFont = XQueryFont( disp, XGContextFromGC( inWnd->wndGC ) );
	XTextExtents( gcFont, inString, textLen, &direction, &ascent, &descent, &overall_return );
	XFreeFontInfo( NULL, gcFont, 0 );

	return ascent + descent;
}


void DrawText( Display *disp, Wnd *inWnd, const char *inString, int x, int y, int maxWidth, int maxHeight, boolean invert )
{
	XFontStruct *gcFont;
	int ascent, descent, direction;
	XCharStruct overall_return;
	int background, foreground;

	int textLen;

	textLen = strlen( inString );

	gcFont = XQueryFont( disp, XGContextFromGC( inWnd->wndGC ) );
	XTextExtents( gcFont, inString, textLen, &direction, &ascent, &descent, &overall_return );
	XFreeFontInfo( NULL, gcFont, 0 );

	if( invert == TRUE )
	{
		XGCValues xgc;

		XGetGCValues( disp, inWnd->wndGC, GCBackground | GCForeground , &xgc );
		foreground = xgc.foreground;
		background = xgc.background;

		XSetForeground( disp, inWnd->wndGC, background );
		XSetBackground( disp, inWnd->wndGC, foreground );
	}

	XDrawImageString( disp, inWnd->wndHandle, inWnd->wndGC, x, y + descent, inString, textLen );

	if( invert == TRUE )
	{
		XSetForeground( disp, inWnd->wndGC, foreground );
		XSetBackground( disp, inWnd->wndGC, background );
	}
}

unsigned int getPixelValue( Display *disp, unsigned int desired )
{
	Colormap defcolmap;
	XColor col;
	int val;

	col.red = ( desired & 0xFF0000 ) >> 16;
	col.green = ( desired & 0xFF00 ) >> 8;
	col.blue = ( desired & 0xFF ); 
	col.flags = DoRed | DoGreen | DoBlue;

	defcolmap = DefaultColormap( disp, DefaultScreen( disp ) );

	if( XAllocColor( disp, defcolmap, &col ) == 0 )
	{
		val = BlackPixel( disp, DefaultScreen( disp ) );
	}
	else
	{
		val = col.pixel;
	}

	return val;
}


/* Computes the widths as percentages of parents heights and uses CreateWindowHelper to create.*/
GC CreateDefaultGC( Display *disp, Wnd *inWnd, unsigned int fore, unsigned int back )
{
	XGCValues xgc;

	/* This is done temporarily disabled while I work the bugs out */
	/*xgc.foreground = getPixelValue( disp, fore ); 
	xgc.background = getPixelValue( disp, back );*/

	xgc.foreground = BlackPixel( disp, DefaultScreen( disp ) );
	xgc.background = WhitePixel( disp, DefaultScreen( disp ) );
	
	return XCreateGC( disp, inWnd->wndHandle, GCForeground | GCBackground, &xgc );
}
