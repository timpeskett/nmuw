#include "AppData.h"


#ifdef __linux
boolean InitAppData( AppData *ad )
{
	XSetWindowAttributes attr;
	XGCValues xgc;

	ad->disp = XOpenDisplay( NULL );
        if( ad->disp == NULL )
	{
		return FALSE;
	}	

	CreateNumWnd( ad, &ad->popWnd );

	return TRUE;
}
#endif


#ifdef __linux
void DestroyAppData( AppData *ad )
{
	DestroyNumWnd( ad, &ad->popWnd );
	XCloseDisplay( ad->disp );
}
#endif
