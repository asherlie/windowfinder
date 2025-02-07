#include <X11/Xlib.h>
#include <stdio.h>

/*
 * Display > Screen > Windows
 */

int main() {
    Display *d;
    Screen* scr;
    int s;

    printf("Attempting to open %s\n", XDisplayName(NULL));
	d = XOpenDisplay(NULL);
    if (!d) {
        printf("Failed to open %s\n", XDisplayName(NULL));
    }
    s = DefaultScreen(d);
    scr = ScreenOfDisplay(d, s);

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
