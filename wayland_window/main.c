#include "window.h"
#include <stdio.h>
#include <wayland-client.h>

int main(){
    struct wl_display *display = wl_display_connect(NULL);

    if (!display) {
        fprintf(stderr, "Failed to connect to Wayland display.\n");
        return 1;
    }

    //This is the event loop
    while (wl_display_dispatch(display) != -1) {

    }

    wl_display_disconnect(display);

    return 0;
}
