#ifndef CLIENT_H
#define CLIENT_H

#ifdef __linux
#include <sys/socket.h>
#endif
#include "../common.h"
#include "../msgs.h"
#include "../netmsg.h"
#include "../sockhelp.h"
#include "../fakeinput.h"
#include "AppData.h"
#include "Screen.h"
#include "NumWindow.h"

#define ADDRLEN 100
#define SLEEP_TIME 10000 /*Time to sleep during each loop*/
#define WAIT_TIME 1 /*Time to wait before checking for connection */

#endif
