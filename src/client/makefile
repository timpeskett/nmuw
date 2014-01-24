CC = gcc
OBJ = client.o NumWindow.o sockhelp.o netmsg.o msgs.o widdata.o serial.o Screen.o fakeinput.o Wnd.o wndutil.o
X11 = -lX11 -lXext -lXtst
CFLAGS = -g -c -pedantic -Wall

client : $(OBJ)
	$(CC) -o client $(OBJ) $(X11)

client.o : client.c client.h ../sockhelp.h ../common.h ../msgs.h ../netmsg.h  AppData.h NumWindow.h Screen.h ../fakeinput.h
	$(CC) $(CFLAGS) client.c

NumWindow.o : NumWindow.h NumWindow.c ../common.h ../wndutil.h
	$(CC) $(CFLAGS) NumWindow.c 

Wnd.o : ../Wnd.h ../Wnd.c ../common.h
	$(CC) -o Wnd.o $(CFLAGS) ../Wnd.c

wndutil.o : ../wndutil.c ../wndutil.h ../Wnd.h ../common.h
	$(CC) -o wndutil.o $(CFLAGS) ../wndutil.c

fakeinput.o : ../fakeinput.h ../fakeinput.c AppData.h ../common.h
	$(CC) -o fakeinput.o $(CFLAGS) ../fakeinput.c

Screen.o : Screen.h Screen.c AppData.h
	$(CC) $(CFLAGS) Screen.c

sockhelp.o : ../sockhelp.h ../sockhelp.c ../common.h
	$(CC) -o sockhelp.o $(CFLAGS) ../sockhelp.c

netmsg.o : ../netmsg.c ../netmsg.h ../common.h ../serial.h ../msgs.h
	$(CC) -o netmsg.o $(CFLAGS) ../netmsg.c

msgs.o : ../msgs.c ../msgs.h ../serial.h ../common.h ../widdata.h
	$(CC) -o msgs.o $(CFLAGS) ../msgs.c

widdata.o : ../widdata.h ../widdata.c
	$(CC) -o widdata.o $(CFLAGS) ../widdata.c

serial.o : ../serial.c ../serial.h ../common.h
	$(CC) -o serial.o $(CFLAGS) ../serial.c


clean :
	rm -f $(OBJ) client
