#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <wayland-client-core.h>
#include <wayland-client.h>
#include "xdg-shell-client-protocol.h"

static struct wl_compositor *compositor;
static struct wl_shm *shm;
static struct wl_surface *surface;

static void registry_add(void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version){
    if (strcmp(interface, "wl_compositor") == 0){
        compositor = wl_registry_bind(registry, name, &wl_compositor_interface, 1);
    }
    else if (strcmp(interface, "wl_shm") == 0){
        shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
    }
}

static void registry_remove(void *data, struct wl_registry *registry, uint32_t name){

}

static const struct wl_registry_listener registry_listener = {
    registry_add,
    registry_remove
};

static struct wl_buffer *create_buffer(int width, int height){
    int stride = width * 4;
    int size = stride * height;

    //Some shared memory thing as far as I understood.
    int fd = shm_open("/wayland-shm", O_CREAT | O_RDWR, 0600);
    ftruncate(fd, size);

    void *data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    uint32_t *pixels = data;
    for (int i = 0; i < width * height; i++){
        pixels[i] = 0xff0066cc;
    }

    struct wl_shm_pool *pool = wl_shm_create_pool(shm, fd, size);

    struct wl_buffer *buffer =  wl_shm_pool_create_buffer(pool, 0, width, height, stride, WL_SHM_FORMAT_ARGB8888);

    wl_shm_pool_destroy(pool);
    close(fd);

    return buffer;
}

int main(){
    int width = 1280;
    int height = 720;

    struct wl_display *display = wl_display_connect(NULL);

    if (!display) {
        fprintf(stderr, "Failed to connect to Wayland display.\n");
        return 1;
    }
    struct wl_registry *registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, NULL);
    wl_display_roundtrip(display);

    surface = wl_compositor_create_surface(compositor);

    struct wl_buffer *buffer = create_buffer(width, height);

    wl_surface_attach(surface, buffer, 0, 0);
    wl_surface_commit(surface);

    //This is the event loop
    while (1) {
        wl_display_dispatch(display);
    }

    wl_display_disconnect(display);

    return 0;
}
