#include <X11/Xlib.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/*
 * Display > Screen > Windows
 */

struct loc_win_arg{
    Display* d;
    Window w;
    unsigned long color;
    unsigned long original_color;
    uint32_t n_flashes;
    uint32_t flash_duration_us;
};

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
    printf("locating windonw %li of %p\n", w, (void*)d);
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
        XSync(d, 0);
        usleep(flash_duration_us);
        XSetWindowBorder(d, w, original_color);
        XSync(d, 0);
        usleep(flash_duration_us);
    }

	wc.border_width = wa.border_width;
    XConfigureWindow(d, w, CWBorderWidth, &wc);
    XSync(d, 0);
}

void* loc_win_pth(void* varg) {
    struct loc_win_arg* wa = varg;

    locate_window(wa->d, wa->w, wa->color, wa->original_color, wa->n_flashes, wa->flash_duration_us);
    return NULL;
}

void search_windows(Display* d, Screen* scr, char* sterm) {
    Window rw, parent;
    Window* children;
    uint32_t nchildren;
    pthread_t* pth;

    XQueryTree(d, RootWindowOfScreen(scr), &rw, &parent, &children, &nchildren);
    pth = malloc(nchildren * sizeof(pthread_t));

    char* wn;
    struct loc_win_arg wa[nchildren];
    memset(wa, 0, sizeof(struct loc_win_arg) * nchildren);

    for (uint32_t i = 0; i < nchildren; ++i) {
        XFetchName(d, children[i], &wn);
        if (wn && (!sterm || strcasestr(wn, sterm))) {
            if (sterm) {
                wa[i].d = d;
                wa[i].w = children[i];
                wa[i].color = get_rgb_ul(255, 0, 0);
                wa[i].original_color = get_rgb_ul(0, 0, 0);
                wa[i].n_flashes = 10;
                wa[i].flash_duration_us = 200000;
                pthread_create(pth + i, NULL, loc_win_pth, &wa[i]);
            }
            printf("\"%s\"\n", wn);
        }
    }

    for (uint32_t i = 0; i < nchildren; ++i) {
        if (wa[i].d == d) {
            pthread_join(pth[i], NULL);
        }
    }
    free(pth);
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
