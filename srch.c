#include <X11/Xlib.h>
#include <stdio.h>

/*
 * Display > Screen > Window
 *
 */
int main() {
    /*Client* c = m->clients;*/
    Display *d;
    Screen* scr;
    XWindowAttributes wa;

    printf("Attempting to open %s\n", XDisplayName(NULL));
	d = XOpenDisplay(NULL);
    if (!d) {
        printf("Failed to open %s\n", XDisplayName(NULL));
    }
    int s = DefaultScreen(d);
    /*printf("nscreens: %i\n", DisplayWidth(d, s));*/
    scr = ScreenOfDisplay(d, s);
    printf("rd: %i\n", scr->root);
    int w = RootWindowOfScreen(scr);
    Window rw, parent;
    Window* children;
    int nchildren;

    XQueryTree(d, RootWindowOfScreen(scr), &rw, &parent, &children, &nchildren);

    printf("%i total windows\n", nchildren);

    char* wn;
    for (int i = 0; i < nchildren; ++i) {
        XFetchName(d, children[i], &wn);
        if (wn) printf("%i: %s\n", i, wn);
    }

}
