#define _POSIX_C_SOURCE 200112L
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <wayland-client.h>
#include "xdg-shell-client-protocol.h"

static inline int64_t my_ftruncate(int32_t fd, uint64_t length) {
    int64_t ret;

    asm volatile (
        "syscall"
        : "=a"(ret)
        : "a"(SYS_ftruncate), "D"(fd), "S"(length)
        : "rcx", "r11", "memory"
    );

    return ret;
}

static inline uint32_t allocate_shared_mem_file(char* name, uint64_t size){
    //uint32_t fd = create_shared_mem_file();
    //char name[] = "SharedFileMemory";
    int32_t fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0600);
    if (fd < 0){
        return 0;
    }
    shm_unlink(name);

    int32_t ret = my_ftruncate(fd, size);//ftruncate(fd, size);

    if (ret < 0) {
        shm_unlink(name);
        return 0;
    }
    return fd;
}

struct ClientState {
    struct wl_shm* wl_shm;
    struct wl_compositor* wl_compositor;
    struct xdg_wm_base* xdg_wm_base;
    /* Objects */
    struct wl_surface* wl_surface;
    struct xdg_surface* xdg_surface;
    struct xdg_toplevel* xdg_toplevel;
};

static void wl_buffer_release(void* data, struct wl_buffer* wl_buffer){
    /* Sent by the compositor when it's no longer using this buffer */
    wl_buffer_destroy(wl_buffer);
}

static const struct wl_buffer_listener wl_buffer_listener = {
    .release = wl_buffer_release,
};


static inline uint32_t window_resize()
{
    return 0;
}


static inline struct wl_buffer* draw_frame(struct ClientState* state){
    const uint32_t width = 640;
    const uint32_t height = 480;
    uint32_t stride = width * 4;
    uint32_t size = stride * height;

    uint32_t fd = allocate_shared_mem_file("SharedFileMemory", size);

    if (!fd) {
        return 0;
    }

    uint32_t* data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (data == MAP_FAILED) {
        return 0;
    }

    struct wl_shm_pool* pool = wl_shm_create_pool(state->wl_shm, fd, size);
    struct wl_buffer* buffer = wl_shm_pool_create_buffer(pool, 0, width, height, stride, WL_SHM_FORMAT_XRGB8888);
    wl_shm_pool_destroy(pool);

    /* Draw checkerboxed background */
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if ((x + y / 8 * 8) % 16 < 8){
                data[y * width + x] = 0;//Black
            }
            else{
                data[y * width + x] = 255;//Blue
            }
        }
    }

    munmap(data, size);
    wl_buffer_add_listener(buffer, &wl_buffer_listener, NULL);
    return buffer;
}

static inline void xdg_surface_configure(void* data, struct xdg_surface* xdg_surface, uint32_t serial){
    struct ClientState* state = data;
    struct wl_buffer* buffer = draw_frame(state);

    if(!buffer) return;

    xdg_surface_ack_configure(xdg_surface, serial);
    wl_surface_attach(state->wl_surface, buffer, 0, 0);
    wl_surface_commit(state->wl_surface);
}


static inline void xdg_wm_base_ping(void* data, struct xdg_wm_base* xdg_wm_base, uint32_t serial){
    xdg_wm_base_pong(xdg_wm_base, serial);
}

static const struct xdg_wm_base_listener xdg_wm_base_listener = {
    .ping = xdg_wm_base_ping,
};

static inline void registry_global(void* data, struct wl_registry* wl_registry, uint32_t name, const char* interface, uint32_t version){
    struct ClientState* state = data;
    if (strcmp(interface, wl_shm_interface.name) == 0) {
        state->wl_shm = wl_registry_bind(wl_registry, name, &wl_shm_interface, 1);
    }
    else if (strcmp(interface, wl_compositor_interface.name) == 0) {
        state->wl_compositor = wl_registry_bind(wl_registry, name, &wl_compositor_interface, 4);
    }
    else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
        state->xdg_wm_base = wl_registry_bind(wl_registry, name, &xdg_wm_base_interface, 1);

        xdg_wm_base_add_listener(state->xdg_wm_base, &xdg_wm_base_listener, state);
    }
}

static inline void registry_global_remove(void* data, struct wl_registry* wl_registry, uint32_t name){
    /* This space deliberately left blank */
}


int main(){
    struct xdg_surface_listener xdg_surface_listener = {
        .configure = xdg_surface_configure,
    };
    struct wl_registry_listener wl_registry_listener = {
        .global = registry_global,
        .global_remove = registry_global_remove,
    };

    struct wl_display* wl_display = wl_display_connect(NULL);
    struct wl_registry* wl_registry = wl_display_get_registry(wl_display);

    struct ClientState state = { 0 };

    wl_registry_add_listener(wl_registry, &wl_registry_listener, &state);
    wl_display_roundtrip(wl_display);

    state.wl_surface = wl_compositor_create_surface(state.wl_compositor);
    state.xdg_surface = xdg_wm_base_get_xdg_surface(state.xdg_wm_base, state.wl_surface);

    xdg_surface_add_listener(state.xdg_surface, &xdg_surface_listener, &state);
    state.xdg_toplevel = xdg_surface_get_toplevel(state.xdg_surface);

    xdg_toplevel_set_title(state.xdg_toplevel, "Example client");
    wl_surface_commit(state.wl_surface);

    while (wl_display_dispatch(wl_display)) {
        /* This space deliberately left blank */
    }

    return 0;
}
