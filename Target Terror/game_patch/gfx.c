//
// Target Terror Graphical Patches
//


#include "global.h"
#include "gfx.h"
#include "io.h"
#include <sys/time.h>
#include <GL/freeglut.h>
#include "rdtsc.h"




/*
Screen Shake Patch
Notes: Screen shake sometimes resizes the drawable area for some reason, causing the render
area to shrink in size. In windowed mode, resizing the window will fix this,
but not on fullscreen.
Partial Mitigation: Disable the effect for now.
Actual Mitigation: Modify ScrShake_Stop() to reset the drawable area somehow.
*/

#define SSSys 0x08144F7C

// For testing, set this when ready to fix the screenshake issue.
int ScrShake_Start(int a1, int a2, int a3) {
    //rfx_uint32_set(SSSys,a1);
    //rfx_uint32_set(0x08144F88,a2);
    //rfx_uint32_set(0x08144F80,a3);
    //rfx_uint32_set(0x08144F84,0);
    return a3;
}

int ScrShake_Update(float a1) {
    // TODO: Add the check against SSSys and update the screen
    // values (GFX_ShakeOffsetX,GFX_ShakeOffsetY) properly.
    return 0;
}

// This is literally all the real function does as well - stops the "screenshake flag"
void ScrShake_Stop() {
    rfx_uint32_set(SSSys,0);
}

// ---[Screen Initialization Patches]---

/*
// The game normally overtakes our cursor in game_mode and makes it invisible, this lets us keep our cursor.
FGAPI void FGAPIENTRY glutSetCursor(int cursor) {
  printf("Cursor reset override from %i\n",cursor);
  return;
}
*/

// VSYNC and Fullscreen Patches
#define GFX_FRAMERATE 60
FGAPI int FGAPIENTRY glutEnterGameMode(void) {
    rfx_printf("Drawing window instead of entering game mode...\n");
    glutCreateWindow("Target Terror: GOLD");
    // If the user specified an environment variable, we go fullscreen.
    // The game does have an option in prefs.txt, but it no longer works.
    if(getenv("TTG_FULLSCREEN"))
        glutFullScreen();
    // Tells glut to set our rendering framerate - VSYNC Method 1.
    setFPS(GFX_FRAMERATE);
    return 1;
}

// Rendering timer callback for VSYNC - Method 1.
static unsigned int redisplay_interval;
void timer() {
    glutPostRedisplay();
    glutTimerFunc(redisplay_interval, timer, 0);
}
void setFPS(unsigned int fps) {
    redisplay_interval = 1000 / fps;
    glutTimerFunc(redisplay_interval, timer, 0);
}

// VSYNC FORCE PATCH - Method 2
static unsigned int vsync_counter;
static long last_time_ms;
#define FRAMERATE 60
void wait_for_vsync(){
    struct timeval current;
    long mtime, seconds, useconds;
    gettimeofday(&current, NULL);
    mtime = ((current.tv_sec) * 1000 + current.tv_usec/1000.0) + 0.5;
    if(mtime < last_time_ms + 7){
        long diff = (7 -(mtime-last_time_ms)) % 7;
        usleep(diff * 1000);
    }

    last_time_ms = mtime;
}
// Some old school games used the cpu counter to calculate the game's speed, this doesn't work too well on 64 bit
// systems, anymore. This janky-ass code is designed to basically sleep during the systime polling, it's dangerous
// and doesn't play well with multiple threads, especially if those threads aren't using systime to determine their
// speed of execution - use only as a last resort and point away from face.

unsigned long long rtTime_ReadSysTime(struct systime *a1)
{
    unsigned long long v1;
    unsigned long long v2;
    unsigned long long result;

    wait_for_vsync();
    v1 = rdtsc();
    v2 = (unsigned int)v1;
    memset(&v1,0x00,4);
    result = (v2 + v1 - rfx_uint64_get(0x0814EDE0) / rfx_uint64_get(0x0814EDE8));
    a1->idk_4 = result;
    a1->idk_0 = 0;
    return result;
}







