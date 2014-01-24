#include "fakeinput.h"


Bool isTestsAvailable( Display *disp )
{
	int event_base, minor_version, major_version, error;
	static int avail = -1;
	Bool result;

	if( avail == -1 )
	{
		if( XTestQueryExtension( disp, &event_base, &error, &major_version, &minor_version ) == True )
		{
			avail = 1;
		}
		else
		{
			avail = 0;
		}
	}

	if( avail == 1 )
	{
		result = True;
	}
	else if( avail == 0 )
	{
		result = False;
	}

	return result;
}


void hidePointer( Display *disp )
{
	int screenx, screeny;

	screenx = DisplayHeight( disp, DefaultScreen( disp ) );
	screeny = DisplayWidth( disp, DefaultScreen( disp ) );

	setPointerPos( disp, screenx, screeny );
}


void setPointerPos( Display *disp, int x, int y )
{
	XWarpPointer( disp, None, RootWindow( disp, DefaultScreen( disp ) ), 0, 0, 0, 0, x, y );
}


void movePointer( Display *disp, int dx, int dy )
{
	XWarpPointer( disp, None, None, 0, 0, 0, 0, dx, dy );
}

void pointerPress( Display *disp, int button )
{
	if( isTestsAvailable( disp ) )
	{
		XTestFakeButtonEvent( disp, button, True, CurrentTime );
	}
	else
	{
		XEvent btn;

		memset( &btn, 0, sizeof( btn ) );

		btn.xbutton.type = ButtonPress;
		btn.xbutton.button = button;
		btn.xbutton.same_screen = True;

		btn.xbutton.subwindow = DefaultRootWindow( disp );
		while( btn.xbutton.subwindow )
		{
			btn.xbutton.window = btn.xbutton.subwindow;
			XQueryPointer( disp, btn.xbutton.window, &btn.xbutton.root, &btn.xbutton.subwindow, &btn.xbutton.x_root, &btn.xbutton.y_root, &btn.xbutton.x, &btn.xbutton.y, &btn.xbutton.state );
		}

		XSendEvent( disp, PointerWindow, True, 0xFF, &btn );
	}
}
	

void pointerRelease( Display *disp, int button )
{
	if( isTestsAvailable( disp ) )
	{
		XTestFakeButtonEvent( disp, button, False, CurrentTime );
	}
	else
	{
		XEvent btn;

		memset( &btn, 0, sizeof( btn ) );

		btn.xbutton.type = ButtonRelease;
		btn.xbutton.button = button;
		btn.xbutton.same_screen = True;

		btn.xbutton.subwindow = DefaultRootWindow( disp );
		while( btn.xbutton.subwindow )
		{
			btn.xbutton.window = btn.xbutton.subwindow;
			XQueryPointer( disp, btn.xbutton.window, &btn.xbutton.root, &btn.xbutton.subwindow, &btn.xbutton.x_root, &btn.xbutton.y_root, &btn.xbutton.x, &btn.xbutton.y, &btn.xbutton.state );
		}

		XSendEvent( disp, PointerWindow, True, 0xFF, &btn );
	}
}


void buttonPress( Display *disp, int button )
{
	if( isTestsAvailable( disp ) )
	{
		XTestFakeKeyEvent( disp, XKeysymToKeycode( disp, button ), True, CurrentTime );
	}
	else
	{
	}
}


void buttonRelease( Display *disp, int button )
{
	if( isTestsAvailable( disp ) )
	{
		XTestFakeKeyEvent( disp, XKeysymToKeycode( disp, button ) , False, CurrentTime );
	}
	else
	{
	}
}


