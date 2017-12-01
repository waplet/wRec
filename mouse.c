#include <stdio.h>
#include <X11/Xlib.h>

char *key_name[] = {
    "first",
    "second (or middle)",
    "third",
    "fourth",  // :D
    "fivth"    // :|
};

int main(int argc, char **argv)
{
    Display *display;
    XEvent xevent;
    Window window;
    int done = 0, count = 0;
    int rx = 0, ry = 0, rw = 0, rh = 0, btn_pressed = 0;
    int rect_x = 0, rect_y = 0, rect_w = 0, rect_h = 0;
    XGCValues gcval;
    GC gc;
    Cursor cursor;

    // Check if display available
    if( (display = XOpenDisplay(NULL)) == NULL )
        return -1;

    window = DefaultRootWindow(display);

    cursor = XCreateFontCursor(display, 68);
    gcval.foreground = XWhitePixel(display, 0);
    gcval.function = GXxor;
    gcval.background = XBlackPixel(display, 0);
    gcval.plane_mask = gcval.background ^ gcval.foreground;
    gcval.subwindow_mode = IncludeInferiors;

    // The XCreateGC() function creates a graphics context and returns a GC.
    gc = XCreateGC(
        display,
        window,
        GCFunction | GCForeground | GCBackground | GCSubwindowMode,
        &gcval);

    XAllowEvents(display, AsyncBoth, CurrentTime);

    XGrabPointer(display,
       window,
       1,
       PointerMotionMask | ButtonMotionMask | ButtonPressMask | ButtonReleaseMask ,
       GrabModeAsync,
       GrabModeAsync,
       None,
       cursor,
       CurrentTime);
    XGrabKeyboard(display,
        window,
        False,
        GrabModeAsync,
        GrabModeAsync,
        CurrentTime);

    while(!done) {
        XNextEvent(display, &xevent);

        switch (xevent.type) {
            case MotionNotify:
                if (btn_pressed) {
                    if (rect_w) {
                        XDrawRectangle(display, window, gc, rect_x, rect_y, rect_w, rect_h);
                    }

                    rect_x = rx;
                    rect_y = ry;
                    rect_w = xevent.xmotion.x - rect_x;
                    rect_h = xevent.xmotion.y - rect_y;
                }

                if (rect_w < 0) {
                    rect_x += rect_w;
                    rect_w = 0 - rect_w;
                }
                if (rect_h < 0) {
                    rect_y += rect_h;
                    rect_h = 0 - rect_h;
                }
                /* draw rectangle */
                XDrawRectangle(display, window, gc, rect_x, rect_y, rect_w, rect_h);
                XFlush(display);
                break;
            // case MotionNotify:
                // printf("Mouse move      : [%d, %d]\n", xevent.xmotion.x_root, xevent.xmotion.y_root);
                // break;
            case ButtonPress:
                // printf("Button pressed  : %s [%d, %d]\n", key_name[xevent.xbutton.button - 1], xevent.xmotion.x_root, xevent.xmotion.y_root);
                btn_pressed = 1;
                rx = xevent.xbutton.x;
                ry = xevent.xbutton.y;
                // Something target related??
                break;
            case ButtonRelease:
                // printf("Button released : %s [%d, %d]\n", key_name[xevent.xbutton.button - 1], xevent.xmotion.x_root, xevent.xmotion.y_root);
                done = 1;
                break;
            case KeyPress:
                done = 2;
                break;
            default:
                break;
        }
    }

    if (rect_w) {
        XDrawRectangle(display, window, gc, rect_x, rect_y, rect_w, rect_h);
        XFlush(display);
    }

    XUngrabPointer(display, CurrentTime);
    XUngrabKeyboard(display, CurrentTime);
    XFreeCursor(display, cursor);
    XFreeGC(display, gc);
    XSync(display, True);

    if (done == 1) {
        printf("%d,%d %dx%d", rect_x, rect_y, rect_w, rect_h);
    } else {
        // If any keypressed, lets abort drawing, by returning -1
        printf("-1");
    }

    return 0;
}
