/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.

 *
 *
 */

#if defined( Uses_TScreen ) && !defined( __TScreen )
#define __TScreen

#include <signal.h>

class TDisplay
{

public:

    friend class TView;

    enum videoModes
        {
        smBW80      = 0x0002,
        smCO80      = 0x0003,
        smMono      = 0x0007,
        smFont8x8   = 0x0100
        };

    static void clearScreen( uchar, uchar );

    static ushort getRows();
    static ushort getCols();

    // This version sets the mode through an external program
    static ushort getCrtMode();
    static void GetCursor(int &x, int &y);
    static void SetCursor(int x, int y);
    static void setCursorType( ushort );
    static ushort getCursorType();
    static void SetPage(uchar);
    static uchar GetPage() { return Page; }
    static int    SelectFont(int height, int noForce=0, int modeRecalculate=1, int width=8);
    static void   RestoreDefaultFont(void);
    static int    CheckForWindowSize(void);

protected:

    TDisplay() {};
    TDisplay( const TDisplay& ) {};
    // SET: Remove the font. I forgot to do it when added font support.
    ~TDisplay() { };

    // SET: 1 when the size of the window where the program is running changed
    static volatile sig_atomic_t windowSizeChanged;

private:

    static ushort * equipment;
    static uchar * crtInfo;
    static uchar * crtRows;
    static uchar Page;

};

class TScreen : public TDisplay
{

public:

    TScreen();
    ~TScreen();

    static void setVideoMode( ushort mode );
    static void setVideoMode( char *mode );
    static void clearScreen();

    static ushort startupMode;
    static ushort startupCursor;
    static ushort screenMode;
    static uchar screenWidth;
    static uchar screenHeight;
    static Boolean hiResScreen;
    static Boolean checkSnow;
    // SET: Changed to avoid problems, in fact some casts to int failed in
    // Alpha/Linux. We found it with Dim Zegebart.
    static ushort *screenBuffer;
    // SET: Indicates if the screen was initialized by the TScreen constructor
    static char initialized;
    static char suspended;
    static ushort cursorLines;

    static void setCrtData();

    static void suspend();
    static void resume();
    static void GetCursor(int &x, int &y);
    static void SetCursor(int x, int y);
    static void setCursorType( ushort );
    static ushort getCursorType();
    static ushort getRows();
    static ushort getCols();

    static ushort getCharacter(unsigned offset);
    static void getCharacter(unsigned offset,ushort *buf,unsigned count);
    static void setCharacter(unsigned offset,ushort value);
    static void setCharacter(unsigned offset,ushort *values,unsigned count);

    // SET: That's very low level, don't call it from any place!
    static void SendToTerminal(const char *value);

    friend void TV_WindowSizeChanged(int sig);
};

// SET: Added the following functionallity:
// Linux specific stuff, enclose it in __linux__ section:
//   If you want to setup a specific value define this variable in your code
// and assign a value. If the variable isn't in your code the linker will use
// a definition found in the library.
extern unsigned TurboVision_screenOptions;

//   This value makes the screen driver try to provide a mode that supports
// a switch to the user screen.
//   Current example: if the TScreen class have access to the /dev/vcsaN
// devices for writing we can't provide user screen (no way to read the
// screen and tcsetattr or endwin() fails). Even with that it's better than
// using escape sequences, but RHIDE will work better if can switch. So I'm
// adding it so Robert can ask for it (or not).
const unsigned TurboVision_screenUserScreenNeeded=1;

// SET: Moved/Redefined
// Internally used by Linux driver, don't use
extern int vcsWfd;          /* virtual console system descriptor */
extern int vcsRfd;          /* SET: Same for reading */
extern int tty_fd;          /* tty descriptor */
#define canWriteVCS     (vcsWfd>=0)
#define canReadVCS      (vcsRfd>=0)
#define canOnlyWriteVCS (vcsWfd>=0 && vcsRfd<0)
#endif  // Uses_TScreen
