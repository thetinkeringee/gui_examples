/// @file main.c

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xcb_util.h>
#include <xcb/xproto.h>

static struct {
  xcb_connection_t *connection;
  int32_t screenNumber;
  xcb_screen_t *screen;

} xcb = {};

// Define the backgrouund color
//
// ALPHA RED GREEN BLUE
#define BG_COLOR 0xA0404050




typedef struct {
    xcb_depth_t* depth;
    xcb_visualtype_t* visual;
    xcb_colormap_t colormap;

} VisualConfig;

/// 
/// This function search for the requested depth. It then searches for an
///
/// appropriate visual. Finally it creates a color map for the
/// depth and visual. 
///
///
int findDepthAndVisual(   //
    xcb_connection_t *c,  ///> server connection
    xcb_screen_t *screen, /// xcb Screen
    const uint8_t depth,  /// The desired screen depth i.e. 24 or 32
    VisualConfig *cfg     /// structure in which to write results
) {

  // Get an iterator for the screen's available depths
  xcb_depth_iterator_t d = xcb_screen_allowed_depths_iterator(screen);

  bool depthFound = false;
  while (d.rem) {
    // Find an xcb_depth_t with the correct depth and that has visuals
    if (d.data->depth == depth && d.data->visuals_len) {
      cfg->depth = d.data;
      depthFound = true;
      break;
    }
    xcb_depth_next(&d);
  }
  if (!depthFound) {
    return -1;
  }

  bool visualFound = false;
  // Search for a suitable visual
  for (xcb_visualtype_iterator_t v = xcb_depth_visuals_iterator(cfg->depth);
       v.rem; xcb_visualtype_next(&v)) {
    if (v.data->_class == XCB_VISUAL_CLASS_TRUE_COLOR) {
      cfg->visual = v.data;
      visualFound = true;
      break;
    }
  }
  if (!visualFound) {
    fprintf(stderr, "Failed to find visual");
    return -2;
  }

  cfg->colormap = xcb_generate_id(c);
  xcb_void_cookie_t cookie =
      xcb_create_colormap_checked(c, XCB_COLORMAP_ALLOC_NONE, cfg->colormap,
                                  screen->root, cfg->visual->visual_id);

  xcb_generic_error_t *error = xcb_request_check(c, cookie);
  if (error) {
    fprintf(stderr, "Fail to create colormap\n");
    return -3;
  }

  return 0;
}

int main(void) {

  // This will connect to the default display
  xcb.connection = xcb_connect(nullptr, &xcb.screenNumber);

  // get the screen
  // This function can be repleace with a for loop iterating over
  // the screens.
  xcb.screen = xcb_aux_get_screen(xcb.connection, xcb.screenNumber);

  // push all commands to the server
  xcb_flush(xcb.connection);

  //  Check to see if we were able to connect properly
  if (xcb_connection_has_error(xcb.connection)) {
    fprintf(stderr, "Error with connection to X11 server\n");
    return -1;
  }

  // Print some information just because we can
  printf("Your screen is %d x %d pixels\n", xcb.screen->width_in_pixels,
         xcb.screen->height_in_pixels);


 VisualConfig cfg = {};

  if( findDepthAndVisual(xcb.connection, xcb.screen, 32, &cfg) ) {
      fprintf(stderr, "Error finding depth and visul\n");
      xcb_disconnect(xcb.connection);
      return -1;
  }

  // The value mask specifies what information is being pased in values to the
  // server
  uint32_t valueMask = XCB_CW_BACK_PIXEL |   // Specify color for background
                       XCB_CW_BORDER_PIXEL | // Specify border pixel color
                       XCB_CW_EVENT_MASK |   // Specify events to receive
                       XCB_CW_COLORMAP;

  // Values to pass to the server
  // They must be in the order from least to highest mask value
  // XCB_CW_BACK_PIXEL = 2
  // XCB_CW_BORDER_PIXEL = 8
  // XCB_CW_EVENT_MASK =  2048
  uint32_t values[] = {
      BG_COLOR,                 // background color
      BG_COLOR,                 // Border color
      XCB_EVENT_MASK_KEY_PRESS, // Receive key press events
      cfg.colormap              //
  };

  // generate an id for our window
  xcb_window_t window1 = xcb_generate_id(xcb.connection);

#define WIN_WIDTH 400
#define WIN_HEIGHT 300


  xcb_create_window(xcb.connection,       // connection to the X11 server
                    cfg.depth->depth, // Use the same depth as the parent
                    window1,              // Id of window to create
                    xcb.screen->root,     // Parent window id
                    0,                    // Window x postion
                    0,                    // Winodw y position
                    WIN_WIDTH,            // Window width
                    WIN_HEIGHT,           // Window height
                    1,                    // border width
                    XCB_WINDOW_CLASS_INPUT_OUTPUT, //
                    cfg.visual->visual_id,       //
                    valueMask, // Specify which values will be pased to server
                    values     // The actual values
  );

  // Get the correct atom for WM_PROTOCOLS
  xcb_intern_atom_cookie_t internAtomCookie =
      xcb_intern_atom(xcb.connection, 1, 12, "WM_PROTOCOLS");
  xcb_intern_atom_reply_t *reply =
      xcb_intern_atom_reply(xcb.connection, internAtomCookie, nullptr);
  xcb_atom_t wm_protocols = reply->atom;
  if (wm_protocols == 0) {
    fprintf(stderr, "Unable to get WM_PROTOCOLS atom\n");
  }
  free(reply);

  // Get the correct atom for WM_DELETE_WINDOW
  internAtomCookie = xcb_intern_atom(xcb.connection, 1, 16, "WM_DELETE_WINDOW");
  reply = xcb_intern_atom_reply(xcb.connection, internAtomCookie, nullptr);
  xcb_atom_t wm_delete_window = reply->atom;
  if (wm_delete_window == 0) {
    fprintf(stderr, "Unable to get WM_DELETE_WINDOW atom\n");
  }
  free(reply);

  xcb_change_property(xcb.connection, XCB_PROP_MODE_REPLACE, window1,
                      wm_protocols, XCB_ATOM_ATOM, 32, 1, &wm_delete_window);

  // Making the window fixed size by setting the WM_NOMRAL_HINTS property
  // The window will become fixed size win min and max sizes are equal
  xcb_size_hints_t sizeHints = {
      .flags = // Which properteries are being set or hinted
      XCB_ICCCM_SIZE_HINT_P_MIN_SIZE | XCB_ICCCM_SIZE_HINT_P_MAX_SIZE,
    .max_height = WIN_HEIGHT,
        .min_height = WIN_HEIGHT,
        .max_width = WIN_WIDTH,
        .min_width = WIN_WIDTH,
  };

  xcb_change_property(xcb.connection, XCB_PROP_MODE_REPLACE, window1,
                      XCB_ATOM_WM_NORMAL_HINTS, XCB_ATOM_WM_SIZE_HINTS, 32,
                      sizeof(xcb_size_hints_t) / 4, &sizeHints);

  //
  // Give window a Name
  const char *const wName = "Example 03";
  const uint32_t wNameLen = strlen(wName);

  xcb_change_property(xcb.connection,        // Conection to the X11 server
                      XCB_PROP_MODE_REPLACE, // Replace the property
                      window1,               // The window to be modified
                      XCB_ATOM_WM_NAME,      // Property to replace
                      XCB_ATOM_STRING,       // Type of Data
                      8,        // Data is in 8-bit chunks since it is a string
                      wNameLen, // lenght of data
                      wName     // pointer the the actual data
  );

  // Make the window visiable
  xcb_map_window(xcb.connection, window1);

  xcb_flush(xcb.connection);

  xcb_generic_event_t *event = nullptr;

#define ESCAPE_KEYCODE 9

  bool shouldQuit = false;

  while ((event = xcb_wait_for_event(xcb.connection))) {

    switch ((*event).response_type & 0x7F) {

    case XCB_KEY_PRESS: {
      // The event is a key press. Cast the event to a key press event
      xcb_key_press_event_t *press = (xcb_key_press_event_t *)event;

      // print the keycode received
      printf("Keycode: %d\n", press->detail);

      // If escape is pressed
      if (ESCAPE_KEYCODE == press->detail) {
        shouldQuit = true;
      }
      break;
    }

    case XCB_CLIENT_MESSAGE: {
      xcb_client_message_event_t *cmessage =
          (xcb_client_message_event_t *)event;

      if (cmessage->type == wm_protocols) {
        // Check to see if client message is of type wm_delete_window
        if (cmessage->data.data32[0] == wm_delete_window) {
          // break out of while loop so that the program cleans up and exits
          // the message may also be ignored and the window will stay open
          shouldQuit = true;
        }
      }
      break;
    }

    default:
      printf("\n\nUnknown Message Received\n\n");
      break;
    }

    free(event);
    if (shouldQuit) {
      break;
    }
  }

  xcb_destroy_window(xcb.connection, window1);

  // Close connection and free resources
  xcb_disconnect(xcb.connection);
  return 0;
}
