#include "X11/Xlib.h"

#include <err.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>

#define POS_X 200
#define POS_Y 200
#define WIDTH 800
#define HEIGHT 600
#define BORDER 15
#define MAX_LINES 10

static Display *dpy;
static int scr;
static Window root;
static Window win;
static GC gc;

static struct lines {
    int x1;
    int y1;
    int x2;
    int y2;
} lines[MAX_LINES];
static int current_line = 0;
static int line_count = 0;

static int prev_x = 0;
static int prev_y = 0;

void button_press(XEvent *ev) {
    switch (ev->xbutton.button) {
    case Button1:
        prev_x = ev->xbutton.x;
        prev_y = ev->xbutton.y;
        break;
    default:
        printf("here\n");
        XSetForeground(dpy, gc, BlackPixel(dpy, scr));
        XSetBackground(dpy, gc, WhitePixel(dpy, scr));
        break;
    }
}

void button_release(XEvent *ev) {
    switch (ev->xbutton.button) {
    case Button1:
        lines[current_line].x1 = prev_x;
        lines[current_line].y1 = prev_y;
        lines[current_line].x2 = ev->xbutton.x;
        lines[current_line].y2 = ev->xbutton.y;

        line_count = fmin(line_count + 1, MAX_LINES);
        current_line++;
        if (current_line >= MAX_LINES) {
            current_line = 0;
        }
    }
}

void motion_notify(XEvent *ev) {
    XDrawLine(dpy, ev->xbutton.window, gc, prev_x, prev_y, ev->xbutton.x,
              ev->xbutton.y);
}

static void (*handler[LASTEvent])(XEvent *) = {
    [ButtonPress] = button_press,
    [ButtonRelease] = button_release,
    [MotionNotify] = motion_notify,
};

static void init() {
    dpy = XOpenDisplay(NULL);
    if (!dpy) {
        errx(1, "Can't open display.");
    }

    scr = XDefaultScreen(dpy);
    root = RootWindow(dpy, scr);

    // Window paramaters
    int depth = DefaultDepth(dpy, scr);
    unsigned int class = InputOutput;
    Visual *visual = DefaultVisual(dpy, scr);

    // Window attributes
    unsigned long attributes_mask = CWBackPixel | CWBorderPixel | CWEventMask;
    XSetWindowAttributes window_attributes = {
        .background_pixel = BlackPixel(dpy, scr),
        .border_pixel = WhitePixel(dpy, scr),
        .event_mask = ButtonPressMask | ButtonReleaseMask | ButtonMotionMask |
                      KeyPressMask,
    };

    // Create window and map it to the screen
    win = XCreateWindow(dpy, root, POS_X, POS_Y, WIDTH, HEIGHT, BORDER, depth,
                        class, visual, attributes_mask, &window_attributes);
    XStoreName(dpy, win, "Saske");
    XMapRaised(dpy, win);

    // Create GC
    unsigned long values_mask = GCLineStyle | GCLineWidth | GCCapStyle |
                                GCJoinStyle | GCFillStyle | GCForeground |
                                GCBackground;
    XGCValues xgcv = {
        .line_style = LineSolid,
        .line_width = 1,
        .cap_style = CapButt,
        .join_style = JoinMiter,
        .fill_style = FillSolid,
        .foreground = WhitePixel(dpy, scr),
        .background = BlackPixel(dpy, scr),
    };

    gc = XCreateGC(dpy, win, values_mask, &xgcv);
}

static void run() {
    XEvent ev;

    static int prev_x = 0;
    static int prev_y = 0;

    while (XNextEvent(dpy, &ev) == 0) {
        XClearWindow(dpy, win);

        if (handler[ev.type])
            handler[ev.type](&ev);

        XTextItem items[1];
        items[0].chars = "Saske";
        items[0].nchars = 5;
        items[0].delta = 1;
        items[0].font = None;

        XDrawText(dpy, win, gc, 50, 50, items, 1);

        for (int i = 0; i < line_count; i++) {
            XDrawLine(dpy, win, gc, lines[i].x1, lines[i].y1, lines[i].x2,
                      lines[i].y2);
        }
    }
}

static void terminate() {
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);
}

int main() {
    init();
    run();
    terminate();

    return 0;
}
