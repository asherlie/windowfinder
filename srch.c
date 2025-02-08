#include <X11/Xlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

/*
 * Display > Screen > Windows
 */

unsigned long get_rgb_ul (uint8_t r, uint8_t g, uint8_t b) {
    unsigned long ret = (r << 16) | (g << 8) | b;
    return ret;
}

/*
 * duration is 1000000 // 1 sec
 * n_secs   is 3000000
 * 
 * how many flashes will we have:
 *     3
*/


void locate_window(Display* d, Window w, unsigned long color, unsigned long original_color,  uint32_t n_flashes, uint32_t flash_duration_us) {
    XWindowAttributes wa;
    XWindowChanges wc;

    // TODO: update border width, i'll prob have to use set attribute instead of xsetwindowborder
    XGetWindowAttributes(d, w, &wa);
    /*printf("%i\n", wa.border_width);*/
    // TODO: find a way to get original border pixel value, it's not contained in window attributes. 
    // setting to black for now
    /*XSetWindowAttributes wa_s = {.wqkj};*/
    /*d, w, &wa);*/
	/*XChangeWindowAttributes(dpy, root, CWEventMask|CWCursor, &wa);*/
	wc.border_width = 5;
    XConfigureWindow(d, w, CWBorderWidth, &wc);
    XSync(d, 0);

    for (uint32_t i = 0; i < n_flashes; ++i) {
        XSetWindowBorder(d, w, color);
        XSync(d, 1);
        usleep(flash_duration_us);
        XSetWindowBorder(d, w, original_color);
        XSync(d, 1);
        usleep(flash_duration_us);
    }

	wc.border_width = wa.border_width;
    XConfigureWindow(d, w, CWBorderWidth, &wc);
    XSync(d, 0);
}

void search_windows(Display* d, Screen* scr, char* sterm) {
    Window rw, parent;
    Window* children;
    uint32_t nchildren;

    XQueryTree(d, RootWindowOfScreen(scr), &rw, &parent, &children, &nchildren);

    char* wn;
    for (uint32_t i = 0; i < nchildren; ++i) {
        XFetchName(d, children[i], &wn);
        if (wn && (!sterm || strcasestr(wn, sterm))) {
            /*Pixmap p;*/
            if (sterm) {
                locate_window(d, children[i], get_rgb_ul(255, 0, 0), get_rgb_ul(0, 0, 0), 10, 200000);
            }
            printf("\"%s\"\n", wn);
        }
    }
}

int main(int argc, char** argv) {
    Display *d;
    Screen* scr;
    int s;

	d = XOpenDisplay(NULL);
    if (!d) {
        printf("Failed to open %s\n", XDisplayName(NULL));
    }
    s = DefaultScreen(d);
    scr = ScreenOfDisplay(d, s);

    search_windows(d, scr, argc == 1 ? NULL : argv[1]);
}
