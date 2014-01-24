CC = gcc
OBJ = server.o netmsg.o msgs.o widdata.o serial.o AppWnd.o ListWnd.o sockhelp.o Monitor.o CaptWnd.o Wnd.o fakeinput.o listWndChild.o appWndChild.o wndutil.o
CFLAGS = -g -c -pedantic -Wall -Werror
LIBS = -lX11 -lXext -lXtst

server : $(OBJ)
	$(CC) -o server $(OBJ) $(LIBS)

server.o : server.c server.h ../common.h ../msgs.h ../netmsg.h AppWnd.h ListWnd.h ServData.h CaptWnd.h
	$(CC) $(CFLAGS) server.c

ListWnd.o : ListWnd.c ListWnd.h ../common.h ../sockhelp.h
	$(CC) $(CFLAGS) ListWnd.c

listWndChild.o : ListWnd.h ../common.h ../Wnd.h ../wndutil.h ServData.h children/listWndChild.h children/listWndChild.c
	$(CC) -o listWndChild.o $(CFLAGS) children/listWndChild.c	

AppWnd.o : AppWnd.h AppWnd.c ServData.h ../common.h 
	$(CC) $(CFLAGS) AppWnd.c

appWndChild.o : AppWnd.h ../common.h ../Wnd.h ../wndutil.h ListWnd.h ServData.h children/appWndChild.h children/appWndChild.c
	$(CC) -o appWndChild.o $(CFLAGS) children/appWndChild.c

Wnd.o : ../Wnd.h ../Wnd.c ../common.h
	$(CC) -o Wnd.o $(CFLAGS) ../Wnd.c

CaptWnd.o : CaptWnd.h CaptWnd.c ../common.h ../fakeinput.h
	$(CC) $(CFLAGS) CaptWnd.c

fakeinput.o : ../fakeinput.h ../fakeinput.c ../common.h
	$(CC) -o fakeinput.o $(CFLAGS) ../fakeinput.c

sockhelp.o : ../sockhelp.h ../sockhelp.c ../common.h
	$(CC) -o sockhelp.o $(CFLAGS) ../sockhelp.c

wndutil.o: ../Wnd.h ../common.h ../wndutil.h ../wndutil.c
	$(CC) -o wndutil.o $(CFLAGS) ../wndutil.c

Monitor.o : Monitor.h Monitor.c ../sockhelp.h ../common.h
	$(CC) $(CFLAGS) Monitor.c

netmsg.o : ../netmsg.c ../netmsg.h ../common.h ../serial.h ../msgs.h 
	$(CC) -o netmsg.o $(CFLAGS) ../netmsg.c

msgs.o : ../msgs.c ../msgs.h ../serial.h ../common.h ../widdata.h
	$(CC) -o msgs.o $(CFLAGS) ../msgs.c

widdata.o : ../widdata.h ../widdata.c
	$(CC) -o widdata.o $(CFLAGS) ../widdata.c

serial.o : ../serial.c ../serial.h ../common.h
	$(CC) -o serial.o $(CFLAGS) ../serial.c


clean :
	rm -f $(OBJ) server
