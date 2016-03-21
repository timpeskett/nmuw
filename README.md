Networked Multi-computer Unified Workspace (nmuw)
=================================================

In a school/university computer lab setting there are many computers but the
monitors are often locked down. Even if the monitors are not locked down, the
machines themselves may not have sufficient video inputs for a multi-headed
setup. nmuw attempts to solve this problem.

nmuw is an application designed to allow a user to control many machines using
a single keyboard and mouse. It is intended to be used for simulating
multi-headed setups where they would otherwise not be possible. Currently,
nmuw supports connecting to ten clients (for essentially a ten monitor setup),
but this is a soft limit and there is no reason that it couldn't be changed
apart from some efficiency concerns.

nmuw operates by capturing the keyboard and mouse of the server computer, and
forwarding the input to other machines over the network when applicable. After
starting the application, pressing Ctrl+Alt+Q will bring up the main window,
from which connections can be made and the session can be configured.


## Video Demo

A short video demo of nmuw can be seen at https://youtu.be/2oYwIgcxA_U.
(It is also in the root of the project git repo).

The video demo was intended to be as simple as possible, and so it may leave
the viewer with some questions. An attempt has been made to answer some of
these questions below.

* What does the text in the gedit window say?
   The text in the gedit window is difficult to see on lower quality levels.
The first gedit window says "The keystrokes and mouse movements are being
entered from the keyboard/mouse belonging to the PC whose monitor is shown to
the left.
   All input is passed over the network to this machine.".
   The second gedit window says "The controlling machine is still completely
usable in this configuration."
* Can the monitors be repositioned during use?
   The monitors can be repositioned during use. The main window can be opened
and the configuration of the monitors can be changed at any time.
* Is there any security to stop someone else connecting?
   No. I never felt the need to implement this as it wasn't a problem for my
personal use. The clients do not accept further connection after they have
been connected to a server. If there is physical access to the machine (as is
intended), then the lack of security shouldn't cause any damage so long as
proper precautions are taken.
* Why does the application window look so terrible?
   The application only uses X11, and so it does all of the drawing by itself.
In retrospect, this was a poor decision.


## Limitations/Bugs

* While the program is designed to simulate a multi-headed environment, there
  are some fundamental differences between this setup and an actual
multi-headed environment.
   It is important to remember that each monitor in the setup actually
corresponds to a different computer. The computers work completely separately,
and so processes cannot be transferred between them. This is especially
important for processes that produce audio, as the audio will come from the
machine on which the process is run.
* The algorithm for placing the screens tightly together contains bugs, and
  appears to glitch out often. I have never experienced a crash due to this,
nor has the configuration become difficult to fix. While it's unattractive, it
has never actually caused an issue.
* When the mouse pointer is returned to the server machine, the user must
  click once before input begins to be sent to this screen. This is because
the server works slightly differently to the clients.
* The Clients window pops up behind the main window. This is because the main
  window is designed to sit on top of all of the windows on the system. The
important controls in this window have never been inaccessible, so it's not
too much of a problem. It's definitely not desirable, though.


## Things That Went Wrong In Development

* I aimed for cross-platform in the beginning. For a bigger project this would
  have made sense, but as it was a project that was motivated by personal
needs I should have foreseen that I wouldn't have to motivation to actually
add Windows support. The effort that went into making it cross-platform was
wasted and made the rest of the design much worse.
* Making the GUI with X11. GTK or equivalent should have been used for this
  part of the system. As much of the system required X11 (the mouse/keyboard
grabbing functionality), it seemed like a good idea to reduce dependencies by
using X11 for the windowing too. This was a mistake.
* The networking code. I had some experience with networking by this time, and
  so the networking portion of the code should not have been very difficult.
To challenge myself, I tried to implement a system to pack messages in such a
way that the minimum possible number of bytes would be sent through the
network. From memory, the system was mostly functional but it was a hassle to
use. In retrospect, it was a silly thing to attempt as the program was
intended to run on LANs where bandwidth is not really an issue.


