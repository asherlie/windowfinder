#include <X11/Xlib.h>
#include <stdio.h>
#include <string.h>

/*
 * Display > Screen > Windows
 */

void search_windows(Display* d, Screen* scr, char* sterm) {
    Window rw, parent;
    Window* children;
    int nchildren;

    XQueryTree(d, RootWindowOfScreen(scr), &rw, &parent, &children, &nchildren);

    char* wn;
    for (int i = 0; i < nchildren; ++i) {
        XFetchName(d, children[i], &wn);
        if (wn && (!sterm || strcasestr(wn, sterm))) {
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
