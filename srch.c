#include <X11/Xlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

/*
 * Display > Screen > Windows
 */

unsigned long get_rgb_ul (uint8_t r, uint8_t g, uint8_t b) {
    unsigned long ret = (r << 16) | (g << 8) | b;
    return ret;
}

void locate_window(Display* d, Window* w, unsigned long color, uint16_t flash_duration, uint8_t n_secs) {
    XSetWindowBorder(d, w, color, );
}

void search_windows(Display* d, Screen* scr, char* sterm) {
    Window rw, parent;
    Window* children;
    int nchildren;

    XQueryTree(d, RootWindowOfScreen(scr), &rw, &parent, &children, &nchildren);

    char* wn;
    for (int i = 0; i < nchildren; ++i) {
        XFetchName(d, children[i], &wn);
        if (wn && (!sterm || strcasestr(wn, sterm))) {
            Pixmap p;
            if (sterm) {
                locate_window(d, children[i], get_rgb_ul(255, 0, 0));
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
