#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>

#include <string.h>
#include <stdio.h>
#include <unistd.h>

static void global_announced(void *data, struct wl_registry *wl_registry,
                             uint32_t name, const char *interface,
                             uint32_t version);


static void global_removed(void *data, struct wl_registry *wl_registry,
                           uint32_t name);

static void pixel_format(void *data, struct wl_shm *wl_shm, uint32_t format);


typedef struct WaylandStruct {
    struct wl_display* display;
    struct wl_registry* registry;
    struct wl_registry_listener registry_listener;
    struct wl_compositor* compositor;
    struct wl_shm*  shm;
    struct wl_shm_listener shm_listener;
    // TODO: Add vector to maintain list of supportted formates
    // this could be a structure tied to shm instead of just the display
} Wayland; 

static_assert(sizeof(Wayland) == 56, "");

int main(void) {

   char nnn[L_tmpnam +1];
   tmpnam(nnn);

   printf("%s\n",nnn);



  Wayland wl = {
      .display = NULL,
      .registry = NULL,
      .registry_listener =
          {   // Callbck to handle global object announcments
              .global = global_announced,
               // Callback for when a global object is removed
              .global_remove = global_removed,
          },
      .shm_listener =
          {
              // handler to receive a list of surface formats
              .format = pixel_format,
          },
  };

  // Get a connection to the wayland server
  wl.display = wl_display_connect(NULL);
  if (NULL == wl.display) {
    fprintf(stderr, "Unable to connect to Wayland server.\n");
    return -1;
    }

    // Get a registry 
    wl.registry = wl_display_get_registry(wl.display);
    if (NULL == wl.registry) {
        fprintf(stderr, "Error getting wl_registry\n");
        wl_display_disconnect(wl.display);
        return -1;
    }

    // Add a global registry listeners aka callbacks  
    if (0 !=
        wl_registry_add_listener(wl.registry, &wl.registry_listener, &wl)) {
      fprintf(stderr, "Error adding reqistry listener!\n");
      wl_registry_destroy(wl.registry);
      wl_display_disconnect(wl.display);
      return -1;
    }
    wl_display_roundtrip(wl.display);

    // check that the compositor is available 
    if (NULL == wl.compositor) {
        fprintf(stderr,"No compositor\n");
        wl_registry_destroy(wl.registry);
        wl_display_disconnect(wl.display);
        return -1;
    }

    // create a surface
    struct wl_surface* surface = wl_compositor_create_surface(wl.compositor);
    if (surface) {
      printf("Surface Created\n");
    } else {
      fprintf(stderr, "Error creating surface\n");
      wl_registry_destroy(wl.registry);
      wl_display_disconnect(wl.display);
      return -1;
    }







    wl_surface_destroy(surface);
    wl_display_roundtrip(wl.display);
    wl_registry_destroy(wl.registry);
    wl_display_roundtrip(wl.display);
    wl_display_disconnect(wl.display);
    return 0;
}

/**
 * announce global object
 *
 * Notify the client of global objects.
 *
 * The event notifies the client that a global object with the
 * given name is now available, and it implements the given version
 * of the given interface.
 * @param name numeric name of the global object
 * @param interface interface implemented by the object
 * @param version interface version
 */
static void global_announced(void *data, struct wl_registry *wl_registry,
                             uint32_t name, const char *interface,
                             uint32_t version) {
  // Since a pointer to wl was passed to data when the listener was created
  // that same pointer is pass into this function as data
  Wayland *wl = (Wayland *)data;

  if (strcmp(interface, wl_compositor_interface.name) == 0) {

    wl->compositor =
        wl_registry_bind(wl_registry, name, &wl_compositor_interface, 5);
    printf("Binding wl_compositor Version: %d\n", version);

  } else if (strcmp(interface, wl_shm_interface.name) == 0) {

    wl->shm = wl_registry_bind(wl_registry, name, &wl_shm_interface, 2);
    printf("Binding wl_shm_interface Version available: %d\n", version);
    wl_shm_add_listener(wl->shm, &wl->shm_listener, wl);

  } else {

    printf("Name: %d Interface: %s Version %d\n", name, interface, version);
  }

}

/**
 * announce removal of global object
 *
 * Notify the client of removed global objects.
 *
 * This event notifies the client that the global identified by
 * name is no longer available. If the client bound to the global
 * using the bind request, the client should now destroy that
 * object.
 *
 * The object remains valid and requests to the object will be
 * ignored until the client destroys it, to avoid races between the
 * global going away and a client sending a request to it.
 * @param name numeric name of the global object
 */
static void global_removed(void *data,
		      struct wl_registry *wl_registry,
		      uint32_t name) {
    printf("Global object removed\n\tName: %d\n\n",name);
};


static char *shm_format_to_text(uint32_t format); 

/**
 * pixel format description
 *
 * Informs the client about a valid pixel format that can be used
 * for buffers. Known formats include argb8888 and xrgb8888.
 * @param format buffer pixel format
 */
static void pixel_format(void *data, struct wl_shm *wl_shm, uint32_t format) {
    printf("Format: %s\n", shm_format_to_text(format));
}

static char *shm_format_to_text(enum wl_shm_format format) {
  switch (format) {
    /**
     * 32-bit ARGB format, [31:0] A:R:G:B 8:8:8:8 little endian
     */
  case WL_SHM_FORMAT_ARGB8888:
    return "WL_SHM_FORMAT_ARGB8888: 32-bit ARGB format, [31:0] A:R:G:B 8:8:8:8 "
           "little endian";
    /**
     * 32-bit RGB format, [31:0] x:R:G:B 8:8:8:8 little endian
     */
  case WL_SHM_FORMAT_XRGB8888:
    return "WL_SHM_FORMAT_XRGB8888 32-bit RGB format, [31:0] x:R:G:B 8:8:8:8 "
           "little endian";
    /**
     * 8-bit color index format, [7:0] C
     */
  case WL_SHM_FORMAT_C8:
    return "WL_SHM_FORMAT_C8 8-bit color index format, [7:0] C";
    /**
     * 8-bit RGB format, [7:0] R:G:B 3:3:2
     */
  case WL_SHM_FORMAT_RGB332:
    return "WL_SHM_FORMAT_RGB332 8-bit RGB format, [7:0] R:G:B 3:3:2";
    /**
     * 8-bit BGR format, [7:0] B:G:R 2:3:3
     */
  case WL_SHM_FORMAT_BGR233:
    return "WL_SHM_FORMAT_BGR233 8-bit BGR format, [7:0] B:G:R 2:3:3";
    /**
     * 16-bit xRGB format, [15:0] x:R:G:B 4:4:4:4 little endian
     */
  case WL_SHM_FORMAT_XRGB4444:
    return "WL_SHM_FORMAT_XRGB4444 16-bit xRGB format, [15:0] x:R:G:B 4:4:4:4 "
           "little endian";
    /**
     * 16-bit xBGR format, [15:0] x:B:G:R 4:4:4:4 little endian
     */
  case WL_SHM_FORMAT_XBGR4444:
    return "WL_SHM_FORMAT_XBGR4444 16-bit xBGR format, [15:0] x:B:G:R 4:4:4:4 "
           "little endian";
    /**
     * 16-bit RGBx format, [15:0] R:G:B:x 4:4:4:4 little endian
     */
  case WL_SHM_FORMAT_RGBX4444:
    return "WL_SHM_FORMAT_RGBX4444 16-bit RGBx format, [15:0] R:G:B:x 4:4:4:4 "
           "little endian";
    /**
     * 16-bit BGRx format, [15:0] B:G:R:x 4:4:4:4 little endian
     */
  case WL_SHM_FORMAT_BGRX4444:
    return "WL_SHM_FORMAT_BGRX4444 16-bit BGRx format, [15:0] B:G:R:x 4:4:4:4 "
           "little endian";
    /**
     * 16-bit ARGB format, [15:0] A:R:G:B 4:4:4:4 little endian
     */
  case WL_SHM_FORMAT_ARGB4444:
    return "WL_SHM_FORMAT_ARGB4444 16-bit ARGB format, [15:0] A:R:G:B 4:4:4:4 "
           "little endian";
    /**
     * 16-bit ABGR format, [15:0] A:B:G:R 4:4:4:4 little endian
     */
  case WL_SHM_FORMAT_ABGR4444:
    return "WL_SHM_FORMAT_ABGR4444 16-bit ABGR format, [15:0] A:B:G:R 4:4:4:4 "
           "little endian";
    /**
     * 16-bit RBGA format, [15:0] R:G:B:A 4:4:4:4 little endian
     */
  case WL_SHM_FORMAT_RGBA4444:
    return "WL_SHM_FORMAT_RGBA4444 16-bit RBGA format, [15:0] R:G:B:A 4:4:4:4 "
           "little endian";
    /**
     * 16-bit BGRA format, [15:0] B:G:R:A 4:4:4:4 little endian
     */
  case WL_SHM_FORMAT_BGRA4444:
    return "WL_SHM_FORMAT_BGRA4444 16-bit BGRA format, [15:0] B:G:R:A 4:4:4:4 "
           "little endian";
    /**
     * 16-bit xRGB format, [15:0] x:R:G:B 1:5:5:5 little endian
     */
  case WL_SHM_FORMAT_XRGB1555:
    return "WL_SHM_FORMAT_XRGB1555 16-bit xRGB format, [15:0] x:R:G:B 1:5:5:5 "
           "little endian";
    /**
     * 16-bit xBGR 1555 format, [15:0] x:B:G:R 1:5:5:5 little endian
     */
  case WL_SHM_FORMAT_XBGR1555:
    return "WL_SHM_FORMAT_XBGR1555 16-bit xBGR 1555 format, [15:0] x:B:G:R "
           "1:5:5:5 little endian";
    /**
     * 16-bit RGBx 5551 format, [15:0] R:G:B:x 5:5:5:1 little endian
     */
  case WL_SHM_FORMAT_RGBX5551:
    return "WL_SHM_FORMAT_RGBX5551 16-bit RGBx 5551 format, [15:0] R:G:B:x "
           "5:5:5:1 little endian";
    /**
     * 16-bit BGRx 5551 format, [15:0] B:G:R:x 5:5:5:1 little endian
     */
  case WL_SHM_FORMAT_BGRX5551:
    return "WL_SHM_FORMAT_BGRX5551 16-bit BGRx 5551 format, [15:0] B:G:R:x "
           "5:5:5:1 little endian";
    /**
     * 16-bit ARGB 1555 format, [15:0] A:R:G:B 1:5:5:5 little endian
     */
  case WL_SHM_FORMAT_ARGB1555:
    return "WL_SHM_FORMAT_ARGB1555 16-bit ARGB 1555 format, [15:0] A:R:G:B "
           "1:5:5:5 little endian";
    /**
     * 16-bit ABGR 1555 format, [15:0] A:B:G:R 1:5:5:5 little endian
     */
  case WL_SHM_FORMAT_ABGR1555:
    return "WL_SHM_FORMAT_ABGR1555 16-bit ABGR 1555 format, [15:0] A:B:G:R "
           "1:5:5:5 little endian";
    /**
     * 16-bit RGBA 5551 format, [15:0] R:G:B:A 5:5:5:1 little endian
     */
  case WL_SHM_FORMAT_RGBA5551:
    return "WL_SHM_FORMAT_RGBA5551 16-bit RGBA 5551 format, [15:0] R:G:B:A "
           "5:5:5:1 little endian";
    /**
     * 16-bit BGRA 5551 format, [15:0] B:G:R:A 5:5:5:1 little endian
     */
  case WL_SHM_FORMAT_BGRA5551:
    return "WL_SHM_FORMAT_BGRA5551 16-bit BGRA 5551 format, [15:0] B:G:R:A "
           "5:5:5:1 little endian";
    /**
     * 16-bit RGB 565 format, [15:0] R:G:B 5:6:5 little endian
     */
  case WL_SHM_FORMAT_RGB565:
    return "WL_SHM_FORMAT_RGB565 16-bit RGB 565 format, [15:0] R:G:B 5:6:5 "
           "little endian";
    /**
     * 16-bit BGR 565 format, [15:0] B:G:R 5:6:5 little endian
     */
  case WL_SHM_FORMAT_BGR565:
    return "WL_SHM_FORMAT_BGR565 16-bit BGR 565 format, [15:0] B:G:R 5:6:5 "
           "little endian";
    /**
     * 24-bit RGB format, [23:0] R:G:B little endian
     */
  case WL_SHM_FORMAT_RGB888:
    return "WL_SHM_FORMAT_RGB888 24-bit RGB format, [23:0] R:G:B little endian";
    /**
     * 24-bit BGR format, [23:0] B:G:R little endian
     */
  case WL_SHM_FORMAT_BGR888:
    return "WL_SHM_FORMAT_BGR888 24-bit BGR format, [23:0] B:G:R little endian";
    /**
     * 32-bit xBGR format, [31:0] x:B:G:R 8:8:8:8 little endian
     */
  case WL_SHM_FORMAT_XBGR8888:
    return "WL_SHM_FORMAT_XBGR8888 32-bit xBGR format, [31:0] x:B:G:R 8:8:8:8 "
           "little endian";
    /**
     * 32-bit RGBx format, [31:0] R:G:B:x 8:8:8:8 little endian
     */
  case WL_SHM_FORMAT_RGBX8888:
    return "WL_SHM_FORMAT_RGBX8888 32-bit RGBx format, [31:0] R:G:B:x 8:8:8:8 "
           "little endian";
    /**
     * 32-bit BGRx format, [31:0] B:G:R:x 8:8:8:8 little endian
     */
  case WL_SHM_FORMAT_BGRX8888:
    return "WL_SHM_FORMAT_BGRX8888 32-bit BGRx format, [31:0] B:G:R:x 8:8:8:8 "
           "little endian";
    /**
     * 32-bit ABGR format, [31:0] A:B:G:R 8:8:8:8 little endian
     */
  case WL_SHM_FORMAT_ABGR8888:
    return "WL_SHM_FORMAT_ABGR8888 32-bit ABGR format, [31:0] A:B:G:R 8:8:8:8 "
           "little endian";
    /**
     * 32-bit RGBA format, [31:0] R:G:B:A 8:8:8:8 little endian
     */
  case WL_SHM_FORMAT_RGBA8888:
    return "WL_SHM_FORMAT_RGBA8888 32-bit RGBA format, [31:0] R:G:B:A 8:8:8:8 "
           "little endian";
    /**
     * 32-bit BGRA format, [31:0] B:G:R:A 8:8:8:8 little endian
     */
  case WL_SHM_FORMAT_BGRA8888:
    return "WL_SHM_FORMAT_BGRA8888 32-bit BGRA format, [31:0] B:G:R:A 8:8:8:8 "
           "little endian";
    /**
     * 32-bit xRGB format, [31:0] x:R:G:B 2:10:10:10 little endian
     */
  case WL_SHM_FORMAT_XRGB2101010:
    return "WL_SHM_FORMAT_XRGB2101010 32-bit xRGB format, [31:0] x:R:G:B "
           "2:10:10:10 little endian";
    /**
     * 32-bit xBGR format, [31:0] x:B:G:R 2:10:10:10 little endian
     */
  case WL_SHM_FORMAT_XBGR2101010:
    return "WL_SHM_FORMAT_XBGR2101010 32-bit xBGR format, [31:0] x:B:G:R "
           "2:10:10:10 little endian";
    /**
     * 32-bit RGBx format, [31:0] R:G:B:x 10:10:10:2 little endian
     */
  case WL_SHM_FORMAT_RGBX1010102:
    return "WL_SHM_FORMAT_RGBX1010102 32-bit RGBx format, [31:0] R:G:B:x "
           "10:10:10:2 little endian";
    /**
     * 32-bit BGRx format, [31:0] B:G:R:x 10:10:10:2 little endian
     */
  case WL_SHM_FORMAT_BGRX1010102:
    return "WL_SHM_FORMAT_BGRX1010102 32-bit BGRx format, [31:0] B:G:R:x "
           "10:10:10:2 little endian";
    /**
     * 32-bit ARGB format, [31:0] A:R:G:B 2:10:10:10 little endian
     */
  case WL_SHM_FORMAT_ARGB2101010:
    return "WL_SHM_FORMAT_ARGB2101010 32-bit ARGB format, [31:0] A:R:G:B "
           "2:10:10:10 little endian";
    /**
     * 32-bit ABGR format, [31:0] A:B:G:R 2:10:10:10 little endian
     */
  case WL_SHM_FORMAT_ABGR2101010:
    return "WL_SHM_FORMAT_ABGR2101010 32-bit ABGR format, [31:0] A:B:G:R "
           "2:10:10:10 little endian";
    /**
     * 32-bit RGBA format, [31:0] R:G:B:A 10:10:10:2 little endian
     */
  case WL_SHM_FORMAT_RGBA1010102:
    return "WL_SHM_FORMAT_RGBA1010102 32-bit RGBA format, [31:0] R:G:B:A "
           "10:10:10:2 little endian";
    /**
     * 32-bit BGRA format, [31:0] B:G:R:A 10:10:10:2 little endian
     */
  case WL_SHM_FORMAT_BGRA1010102:
    return "WL_SHM_FORMAT_BGRA1010102 32-bit BGRA format, [31:0] B:G:R:A "
           "10:10:10:2 little endian";
    /**
     * packed YCbCr format, [31:0] Cr0:Y1:Cb0:Y0 8:8:8:8 little endian
     */
  case WL_SHM_FORMAT_YUYV:
    return "WL_SHM_FORMAT_YUYV packed YCbCr format, [31:0] Cr0:Y1:Cb0:Y0 "
           "8:8:8:8 little endian";
    /**
     * packed YCbCr format, [31:0] Cb0:Y1:Cr0:Y0 8:8:8:8 little endian
     */
  case WL_SHM_FORMAT_YVYU:
    return "WL_SHM_FORMAT_YVYU packed YCbCr format, [31:0] Cb0:Y1:Cr0:Y0 "
           "8:8:8:8 little endian";
    /**
     * packed YCbCr format, [31:0] Y1:Cr0:Y0:Cb0 8:8:8:8 little endian
     */
  case WL_SHM_FORMAT_UYVY:
    return "WL_SHM_FORMAT_UYVY packed YCbCr format, [31:0] Y1:Cr0:Y0:Cb0 "
           "8:8:8:8 little endian";
    /**
     * packed YCbCr format, [31:0] Y1:Cb0:Y0:Cr0 8:8:8:8 little endian
     */
  case WL_SHM_FORMAT_VYUY:
    return "WL_SHM_FORMAT_VYUY packed YCbCr format, [31:0] Y1:Cb0:Y0:Cr0 "
           "8:8:8:8 little endian";
    /**
     * packed AYCbCr format, [31:0] A:Y:Cb:Cr 8:8:8:8 little endian
     */
  case WL_SHM_FORMAT_AYUV:
    return "WL_SHM_FORMAT_AYUV packed AYCbCr format, [31:0] A:Y:Cb:Cr 8:8:8:8 "
           "little endian";
    /**
     * 2 plane YCbCr Cr:Cb format, 2x2 subsampled Cr:Cb plane
     */
  case WL_SHM_FORMAT_NV12:
    return "WL_SHM_FORMAT_NV12 2 plane YCbCr Cr:Cb format, 2x2 subsampled "
           "Cr:Cb plane";
    /**
     * 2 plane YCbCr Cb:Cr format, 2x2 subsampled Cb:Cr plane
     */
  case WL_SHM_FORMAT_NV21:
    return "WL_SHM_FORMAT_NV21 2 plane YCbCr Cb:Cr format, 2x2 subsampled "
           "Cb:Cr plane";
    /**
     * 2 plane YCbCr Cr:Cb format, 2x1 subsampled Cr:Cb plane
     */
  case WL_SHM_FORMAT_NV16:
    return "WL_SHM_FORMAT_NV16 2 plane YCbCr Cr:Cb format, 2x1 subsampled "
           "Cr:Cb plane";
    /**
     * 2 plane YCbCr Cb:Cr format, 2x1 subsampled Cb:Cr plane
     */
  case WL_SHM_FORMAT_NV61:
    return "WL_SHM_FORMAT_NV61 2 plane YCbCr Cb:Cr format, 2x1 subsampled "
           "Cb:Cr plane";
    /**
     * 3 plane YCbCr format, 4x4 subsampled Cb (1) and Cr (2) planes
     */
  case WL_SHM_FORMAT_YUV410:
    return "WL_SHM_FORMAT_YUV410 3 plane YCbCr format, 4x4 subsampled Cb (1) "
           "and Cr (2) planes";
    /**
     * 3 plane YCbCr format, 4x4 subsampled Cr (1) and Cb (2) planes
     */
  case WL_SHM_FORMAT_YVU410:
    return "WL_SHM_FORMAT_YVU410 3 plane YCbCr format, 4x4 subsampled Cr (1) "
           "and Cb (2) planes";
    /**
     * 3 plane YCbCr format, 4x1 subsampled Cb (1) and Cr (2) planes
     */
  case WL_SHM_FORMAT_YUV411:
    return "WL_SHM_FORMAT_YUV411 3 plane YCbCr format, 4x1 subsampled Cb (1) "
           "and Cr (2) planes";
    /**
     * 3 plane YCbCr format, 4x1 subsampled Cr (1) and Cb (2) planes
     */
  case WL_SHM_FORMAT_YVU411:
    return "WL_SHM_FORMAT_YVU411 3 plane YCbCr format, 4x1 subsampled Cr (1) "
           "and Cb (2) planes";
    /**
     * 3 plane YCbCr format, 2x2 subsampled Cb (1) and Cr (2) planes
     */
  case WL_SHM_FORMAT_YUV420:
    return "WL_SHM_FORMAT_YUV420 3 plane YCbCr format, 2x2 subsampled Cb (1) "
           "and Cr (2) planes";
    /**
     * 3 plane YCbCr format, 2x2 subsampled Cr (1) and Cb (2) planes
     */
  case WL_SHM_FORMAT_YVU420:
    return "WL_SHM_FORMAT_YVU420 3 plane YCbCr format, 2x2 subsampled Cr (1) "
           "and Cb (2) planes";
    /**
     * 3 plane YCbCr format, 2x1 subsampled Cb (1) and Cr (2) planes
     */
  case WL_SHM_FORMAT_YUV422:
    return "WL_SHM_FORMAT_YUV422 3 plane YCbCr format, 2x1 subsampled Cb (1) "
           "and Cr (2) planes";
    /**
     * 3 plane YCbCr format, 2x1 subsampled Cr (1) and Cb (2) planes
     */
  case WL_SHM_FORMAT_YVU422:
    return "WL_SHM_FORMAT_YVU422 3 plane YCbCr format, 2x1 subsampled Cr (1) "
           "and Cb (2) planes";
    /**
     * 3 plane YCbCr format, non-subsampled Cb (1) and Cr (2) planes
     */
  case WL_SHM_FORMAT_YUV444:
    return "WL_SHM_FORMAT_YUV444 3 plane YCbCr format, non-subsampled Cb (1) "
           "and Cr (2) planes";
    /**
     * 3 plane YCbCr format, non-subsampled Cr (1) and Cb (2) planes
     */
  case WL_SHM_FORMAT_YVU444:
    return "WL_SHM_FORMAT_YVU444 3 plane YCbCr format, non-subsampled Cr (1) "
           "and Cb (2) planes";
    /**
     * [7:0] R
     */
  case WL_SHM_FORMAT_R8:
    return "WL_SHM_FORMAT_R8 [7:0] R";
    /**
     * [15:0] R little endian
     */
  case WL_SHM_FORMAT_R16:
    return "WL_SHM_FORMAT_R16 [15:0] R little endian";
    /**
     * [15:0] R:G 8:8 little endian
     */
  case WL_SHM_FORMAT_RG88:
    return "WL_SHM_FORMAT_RG88 [15:0] R:G 8:8 little endian";
    /**
     * [15:0] G:R 8:8 little endian
     */
  case WL_SHM_FORMAT_GR88:
    return "WL_SHM_FORMAT_GR88 [15:0] G:R 8:8 little endian";
    /**
     * [31:0] R:G 16:16 little endian
     */
  case WL_SHM_FORMAT_RG1616:
    return "WL_SHM_FORMAT_RG1616 [31:0] R:G 16:16 little endian";
    /**
     * [31:0] G:R 16:16 little endian
     */
  case WL_SHM_FORMAT_GR1616:
    return "WL_SHM_FORMAT_GR1616 [31:0] G:R 16:16 little endian";
    /**
     * [63:0] x:R:G:B 16:16:16:16 little endian
     */
  case WL_SHM_FORMAT_XRGB16161616F:
    return "WL_SHM_FORMAT_XRGB16161616F [63:0] x:R:G:B 16:16:16:16 little "
           "endian";
    /**
     * [63:0] x:B:G:R 16:16:16:16 little endian
     */
  case WL_SHM_FORMAT_XBGR16161616F:
    return "WL_SHM_FORMAT_XBGR16161616F [63:0] x:B:G:R 16:16:16:16 little "
           "endian";
    /**
     * [63:0] A:R:G:B 16:16:16:16 little endian
     */
  case WL_SHM_FORMAT_ARGB16161616F:
    return "WL_SHM_FORMAT_ARGB16161616F [63:0] A:R:G:B 16:16:16:16 little "
           "endian";
    /**
     * [63:0] A:B:G:R 16:16:16:16 little endian
     */
  case WL_SHM_FORMAT_ABGR16161616F:
    return "WL_SHM_FORMAT_ABGR16161616F [63:0] A:B:G:R 16:16:16:16 little "
           "endian";
    /**
     * [31:0] X:Y:Cb:Cr 8:8:8:8 little endian
     */
  case WL_SHM_FORMAT_XYUV8888:
    return "WL_SHM_FORMAT_XYUV8888 [31:0] X:Y:Cb:Cr 8:8:8:8 little endian";
    /**
     * [23:0] Cr:Cb:Y 8:8:8 little endian
     */
  case WL_SHM_FORMAT_VUY888:
    return "WL_SHM_FORMAT_VUY888 [23:0] Cr:Cb:Y 8:8:8 little endian";
    /**
     * Y followed by U then V, 10:10:10. Non-linear modifier only
     */
  case WL_SHM_FORMAT_VUY101010:
    return "WL_SHM_FORMAT_VUY101010 Y followed by U then V, 10:10:10. "
           "Non-linear modifier only";
    /**
     * [63:0] Cr0:0:Y1:0:Cb0:0:Y0:0 10:6:10:6:10:6:10:6 little endian per 2 Y
     * pixels
     */
  case WL_SHM_FORMAT_Y210:
    return "WL_SHM_FORMAT_Y210 [63:0] Cr0:0:Y1:0:Cb0:0:Y0:0 "
           "10:6:10:6:10:6:10:6 little endian per 2 Y pixels";
    /**
     * [63:0] Cr0:0:Y1:0:Cb0:0:Y0:0 12:4:12:4:12:4:12:4 little endian per 2 Y
     * pixels
     */
  case WL_SHM_FORMAT_Y212:
    return "WL_SHM_FORMAT_Y212 [63:0] Cr0:0:Y1:0:Cb0:0:Y0:0 "
           "12:4:12:4:12:4:12:4 little endian per 2 Y pixels";
    /**
     * [63:0] Cr0:Y1:Cb0:Y0 16:16:16:16 little endian per 2 Y pixels
     */
  case WL_SHM_FORMAT_Y216:
    return "WL_SHM_FORMAT_Y216 [63:0] Cr0:Y1:Cb0:Y0 16:16:16:16 little endian "
           "per 2 Y pixels";
    /**
     * [31:0] A:Cr:Y:Cb 2:10:10:10 little endian
     */
  case WL_SHM_FORMAT_Y410:
    return "WL_SHM_FORMAT_Y410 [31:0] A:Cr:Y:Cb 2:10:10:10 little endian";
    /**
     * [63:0] A:0:Cr:0:Y:0:Cb:0 12:4:12:4:12:4:12:4 little endian
     */
  case WL_SHM_FORMAT_Y412:
    return "WL_SHM_FORMAT_Y412 [63:0] A:0:Cr:0:Y:0:Cb:0 12:4:12:4:12:4:12:4 "
           "little endian";
    /**
     * [63:0] A:Cr:Y:Cb 16:16:16:16 little endian
     */
  case WL_SHM_FORMAT_Y416:
    return "WL_SHM_FORMAT_Y416 [63:0] A:Cr:Y:Cb 16:16:16:16 little endian";
    /**
     * [31:0] X:Cr:Y:Cb 2:10:10:10 little endian
     */
  case WL_SHM_FORMAT_XVYU2101010:
    return "WL_SHM_FORMAT_XVYU2101010 [31:0] X:Cr:Y:Cb 2:10:10:10 little "
           "endian";
    /**
     * [63:0] X:0:Cr:0:Y:0:Cb:0 12:4:12:4:12:4:12:4 little endian
     */
  case WL_SHM_FORMAT_XVYU12_16161616:
    return "WL_SHM_FORMAT_XVYU12_16161616 [63:0] X:0:Cr:0:Y:0:Cb:0 "
           "12:4:12:4:12:4:12:4 little endian";
    /**
     * [63:0] X:Cr:Y:Cb 16:16:16:16 little endian
     */
  case WL_SHM_FORMAT_XVYU16161616:
    return "WL_SHM_FORMAT_XVYU16161616 [63:0] X:Cr:Y:Cb 16:16:16:16 little "
           "endian";
    /**
     * [63:0]   A3:A2:Y3:0:Cr0:0:Y2:0:A1:A0:Y1:0:Cb0:0:Y0:0
     * 1:1:8:2:8:2:8:2:1:1:8:2:8:2:8:2 little endian
     */
  case WL_SHM_FORMAT_Y0L0:
    return "WL_SHM_FORMAT_Y0L0 [63:0]   "
           "A3:A2:Y3:0:Cr0:0:Y2:0:A1:A0:Y1:0:Cb0:0:Y0:0  "
           "1:1:8:2:8:2:8:2:1:1:8:2:8:2:8:2 little endian";
    /**
     * [63:0]   X3:X2:Y3:0:Cr0:0:Y2:0:X1:X0:Y1:0:Cb0:0:Y0:0
     * 1:1:8:2:8:2:8:2:1:1:8:2:8:2:8:2 little endian
     */
  case WL_SHM_FORMAT_X0L0:
    return "WL_SHM_FORMAT_X0L0 [63:0]   "
           "X3:X2:Y3:0:Cr0:0:Y2:0:X1:X0:Y1:0:Cb0:0:Y0:0  "
           "1:1:8:2:8:2:8:2:1:1:8:2:8:2:8:2 little endian";
    /**
     * [63:0]   A3:A2:Y3:Cr0:Y2:A1:A0:Y1:Cb0:Y0  1:1:10:10:10:1:1:10:10:10
     * little endian
     */
  case WL_SHM_FORMAT_Y0L2:
    return "WL_SHM_FORMAT_Y0L2 [63:0]   A3:A2:Y3:Cr0:Y2:A1:A0:Y1:Cb0:Y0  "
           "1:1:10:10:10:1:1:10:10:10 little endian";
    /**
     * [63:0]   X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0  1:1:10:10:10:1:1:10:10:10
     * little endian
     */
  case WL_SHM_FORMAT_X0L2:
    return "WL_SHM_FORMAT_X0L2 [63:0]   X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0  "
           "1:1:10:10:10:1:1:10:10:10 little endian";
    /**
     * [63:0]   X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0  1:1:10:10:10:1:1:10:10:10
     * little endian
     */
  case WL_SHM_FORMAT_YUV420_8BIT:
    return "WL_SHM_FORMAT_YUV420_8BIT [63:0]   X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0 "
           " 1:1:10:10:10:1:1:10:10:10 little endian";
    /**
     * [63:0]   X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0  1:1:10:10:10:1:1:10:10:10
     * little endian
     */
  case WL_SHM_FORMAT_YUV420_10BIT:
    return "WL_SHM_FORMAT_YUV420_10BIT [63:0]   "
           "X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0  1:1:10:10:10:1:1:10:10:10 little "
           "endian";
    /**
     * [63:0]   X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0  1:1:10:10:10:1:1:10:10:10
     * little endian
     */
  case WL_SHM_FORMAT_XRGB8888_A8:
    return "WL_SHM_FORMAT_XRGB8888_A8 [63:0]   X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0 "
           " 1:1:10:10:10:1:1:10:10:10 little endian";
    /**
     * [63:0]   X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0  1:1:10:10:10:1:1:10:10:10
     * little endian
     */
  case WL_SHM_FORMAT_XBGR8888_A8:
    return "WL_SHM_FORMAT_XBGR8888_A8 [63:0]   X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0 "
           " 1:1:10:10:10:1:1:10:10:10 little endian";
    /**
     * [63:0]   X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0  1:1:10:10:10:1:1:10:10:10
     * little endian
     */
  case WL_SHM_FORMAT_RGBX8888_A8:
    return "WL_SHM_FORMAT_RGBX8888_A8 [63:0]   X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0 "
           " 1:1:10:10:10:1:1:10:10:10 little endian";
    /**
     * [63:0]   X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0  1:1:10:10:10:1:1:10:10:10
     * little endian
     */
  case WL_SHM_FORMAT_BGRX8888_A8:
    return "WL_SHM_FORMAT_BGRX8888_A8 [63:0]   X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0 "
           " 1:1:10:10:10:1:1:10:10:10 little endian";
    /**
     * [63:0]   X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0  1:1:10:10:10:1:1:10:10:10
     * little endian
     */
  case WL_SHM_FORMAT_RGB888_A8:
    return "WL_SHM_FORMAT_RGB888_A8 [63:0]   X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0  "
           "1:1:10:10:10:1:1:10:10:10 little endian";
    /**
     * [63:0]   X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0  1:1:10:10:10:1:1:10:10:10
     * little endian
     */
  case WL_SHM_FORMAT_BGR888_A8:
    return "WL_SHM_FORMAT_BGR888_A8 [63:0]   X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0  "
           "1:1:10:10:10:1:1:10:10:10 little endian";
    /**
     * [63:0]   X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0  1:1:10:10:10:1:1:10:10:10
     * little endian
     */
  case WL_SHM_FORMAT_RGB565_A8:
    return "WL_SHM_FORMAT_RGB565_A8 [63:0]   X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0  "
           "1:1:10:10:10:1:1:10:10:10 little endian";
    /**
     * [63:0]   X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0  1:1:10:10:10:1:1:10:10:10
     * little endian
     */
  case WL_SHM_FORMAT_BGR565_A8:
    return "WL_SHM_FORMAT_BGR565_A8 [63:0]   X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0  "
           "1:1:10:10:10:1:1:10:10:10 little endian";
    /**
     * non-subsampled Cr:Cb plane
     */
  case WL_SHM_FORMAT_NV24:
    return "WL_SHM_FORMAT_NV24 non-subsampled Cr:Cb plane";
    /**
     * non-subsampled Cb:Cr plane
     */
  case WL_SHM_FORMAT_NV42:
    return "WL_SHM_FORMAT_NV42 non-subsampled Cb:Cr plane";
    /**
     * 2x1 subsampled Cr:Cb plane, 10 bit per channel
     */
  case WL_SHM_FORMAT_P210:
    return "WL_SHM_FORMAT_P210 2x1 subsampled Cr:Cb plane, 10 bit per channel";
    /**
     * 2x2 subsampled Cr:Cb plane 10 bits per channel
     */
  case WL_SHM_FORMAT_P010:
    return "WL_SHM_FORMAT_P010 2x2 subsampled Cr:Cb plane 10 bits per channel";
    /**
     * 2x2 subsampled Cr:Cb plane 12 bits per channel
     */
  case WL_SHM_FORMAT_P012:
    return "WL_SHM_FORMAT_P012 2x2 subsampled Cr:Cb plane 12 bits per channel";
    /**
     * 2x2 subsampled Cr:Cb plane 16 bits per channel
     */
  case WL_SHM_FORMAT_P016:
    return "WL_SHM_FORMAT_P016 2x2 subsampled Cr:Cb plane 16 bits per channel";
    /**
     * [63:0] A:x:B:x:G:x:R:x 10:6:10:6:10:6:10:6 little endian
     */
  case WL_SHM_FORMAT_AXBXGXRX106106106106:
    return "WL_SHM_FORMAT_AXBXGXRX106106106106 [63:0] A:x:B:x:G:x:R:x "
           "10:6:10:6:10:6:10:6 little endian";
    /**
     * 2x2 subsampled Cr:Cb plane
     */
  case WL_SHM_FORMAT_NV15:
    return "WL_SHM_FORMAT_NV15 2x2 subsampled Cr:Cb plane";
    /**
     * 2x2 subsampled Cr:Cb plane
     */
  case WL_SHM_FORMAT_Q410:
    return "WL_SHM_FORMAT_Q410 2x2 subsampled Cr:Cb plane";
    /**
     * 2x2 subsampled Cr:Cb plane
     */
  case WL_SHM_FORMAT_Q401:
    return "WL_SHM_FORMAT_Q401 2x2 subsampled Cr:Cb plane";
    /**
     * [63:0] x:R:G:B 16:16:16:16 little endian
     */
  case WL_SHM_FORMAT_XRGB16161616:
    return "WL_SHM_FORMAT_XRGB16161616 [63:0] x:R:G:B 16:16:16:16 little "
           "endian";
    /**
     * [63:0] x:B:G:R 16:16:16:16 little endian
     */
  case WL_SHM_FORMAT_XBGR16161616:
    return "WL_SHM_FORMAT_XBGR16161616 [63:0] x:B:G:R 16:16:16:16 little "
           "endian";
    /**
     * [63:0] A:R:G:B 16:16:16:16 little endian
     */
  case WL_SHM_FORMAT_ARGB16161616:
    return "WL_SHM_FORMAT_ARGB16161616 [63:0] A:R:G:B 16:16:16:16 little "
           "endian";
    /**
     * [63:0] A:B:G:R 16:16:16:16 little endian
     */
  case WL_SHM_FORMAT_ABGR16161616:
    return "WL_SHM_FORMAT_ABGR16161616 [63:0] A:B:G:R 16:16:16:16 little "
           "endian";
    /**
     * [7:0] C0:C1:C2:C3:C4:C5:C6:C7 1:1:1:1:1:1:1:1 eight pixels/byte
     */
  case WL_SHM_FORMAT_C1:
    return "WL_SHM_FORMAT_C1 [7:0] C0:C1:C2:C3:C4:C5:C6:C7 1:1:1:1:1:1:1:1 "
           "eight pixels/byte";
    /**
     * [7:0] C0:C1:C2:C3 2:2:2:2 four pixels/byte
     */
  case WL_SHM_FORMAT_C2:
    return "WL_SHM_FORMAT_C2 [7:0] C0:C1:C2:C3 2:2:2:2 four pixels/byte";
    /**
     * [7:0] C0:C1 4:4 two pixels/byte
     */
  case WL_SHM_FORMAT_C4:
    return "WL_SHM_FORMAT_C4 [7:0] C0:C1 4:4 two pixels/byte";
    /**
     * [7:0] D0:D1:D2:D3:D4:D5:D6:D7 1:1:1:1:1:1:1:1 eight pixels/byte
     */
  case WL_SHM_FORMAT_D1:
    return "WL_SHM_FORMAT_D1 [7:0] D0:D1:D2:D3:D4:D5:D6:D7 1:1:1:1:1:1:1:1 "
           "eight pixels/byte";
    /**
     * [7:0] D0:D1:D2:D3 2:2:2:2 four pixels/byte
     */
  case WL_SHM_FORMAT_D2:
    return "WL_SHM_FORMAT_D2 [7:0] D0:D1:D2:D3 2:2:2:2 four pixels/byte";
    /**
     * [7:0] D0:D1 4:4 two pixels/byte
     */
  case WL_SHM_FORMAT_D4:
    return "WL_SHM_FORMAT_D4 [7:0] D0:D1 4:4 two pixels/byte";
    /**
     * [7:0] D
     */
  case WL_SHM_FORMAT_D8:
    return "WL_SHM_FORMAT_D8 [7:0] D";
    /**
     * [7:0] R0:R1:R2:R3:R4:R5:R6:R7 1:1:1:1:1:1:1:1 eight pixels/byte
     */
  case WL_SHM_FORMAT_R1:
    return "WL_SHM_FORMAT_R1 [7:0] R0:R1:R2:R3:R4:R5:R6:R7 1:1:1:1:1:1:1:1 "
           "eight pixels/byte";
    /**
     * [7:0] R0:R1:R2:R3 2:2:2:2 four pixels/byte
     */
  case WL_SHM_FORMAT_R2:
    return "WL_SHM_FORMAT_R2 [7:0] R0:R1:R2:R3 2:2:2:2 four pixels/byte";
    /**
     * [7:0] R0:R1 4:4 two pixels/byte
     */
  case WL_SHM_FORMAT_R4:
    return "WL_SHM_FORMAT_R4 [7:0] R0:R1 4:4 two pixels/byte";
    /**
     * [15:0] x:R 6:10 little endian
     */
  case WL_SHM_FORMAT_R10:
    return "WL_SHM_FORMAT_R10 [15:0] x:R 6:10 little endian";
    /**
     * [15:0] x:R 4:12 little endian
     */
  case WL_SHM_FORMAT_R12:
    return "WL_SHM_FORMAT_R12 [15:0] x:R 4:12 little endian";
    /**
     * [31:0] A:Cr:Cb:Y 8:8:8:8 little endian
     */
  case WL_SHM_FORMAT_AVUY8888:
    return "WL_SHM_FORMAT_AVUY8888 [31:0] A:Cr:Cb:Y 8:8:8:8 little endian";
    /**
     * [31:0] X:Cr:Cb:Y 8:8:8:8 little endian
     */
  case WL_SHM_FORMAT_XVUY8888:
    return "WL_SHM_FORMAT_XVUY8888 [31:0] X:Cr:Cb:Y 8:8:8:8 little endian";
    /**
     * 2x2 subsampled Cr:Cb plane 10 bits per channel packed
     */
  case WL_SHM_FORMAT_P030:
    return "WL_SHM_FORMAT_P030 2x2 subsampled Cr:Cb plane 10 bits per channel "
           "packed";
  default:
    return "Unkown Format";
  };
};
