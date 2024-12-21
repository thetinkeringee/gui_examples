#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xcb/xcb.h>
#include <xcb/xcb_util.h>
#include <xcb/xproto.h>

static struct {
  xcb_connection_t *connection;
  int32_t screenNumber;
  xcb_screen_t *screen;

} xcb = {};

// Define the backgrouund color
//
// Alpha will likely have no effect on the display with out
// other changes to the code. This will be shown in anotehr
// example.
//
// ALPHA RED GREEN BLUE
#define BG_COLOR 0xFF404050

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

  // The value mask specifies what information is being pased in values to the
  // server
  uint32_t valueMask = XCB_CW_BACK_PIXEL |   // Specify color for background
                       XCB_CW_BORDER_PIXEL | // Specify border pixel color
                       XCB_CW_EVENT_MASK;    // Specify events to receive

  // Values to pass to the server
  // They must be in the order from least to highest mask value
  // XCB_CW_BACK_PIXEL = 2
  // XCB_CW_BORDER_PIXEL = 8
  // XCB_CW_EVENT_MASK =  2048
  uint32_t values[] = {
      BG_COLOR,                // background color
      BG_COLOR,                // Border color
      XCB_EVENT_MASK_KEY_PRESS // Receive key press events
  };

  // generate an id for our window
  xcb_window_t window1 = xcb_generate_id(xcb.connection);

  xcb_create_window(xcb.connection,       // connection to the X11 server
                    XCB_COPY_FROM_PARENT, // Use the same depth as the parent
                    window1,              // Id of window to create
                    xcb.screen->root,     // Parent window id
                    0,                    // Window x postion
                    0,                    // Winodw y position
                    400,                  // Window width 
                    300,                  // Window height
                    1,                    // border width
                    XCB_WINDOW_CLASS_INPUT_OUTPUT, //
                    xcb.screen->root_visual,       //
                    valueMask, // Specify which values will be pased to server
                    values     // The actual values
  );
  // NOTE: the window size and position may be ignored by some window managers
 

  //
  // Give window a Name
  const char* const wName = "Example 01";
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

  while ((event = xcb_wait_for_event(xcb.connection))) {

    if ((event->response_type & ~80) == XCB_KEY_PRESS) {
      // The event is a key press. Cast the event to a key press event
      xcb_key_press_event_t *press = (xcb_key_press_event_t *)event;

      // print the keycode received
      printf("Keycode: %d\n", press->detail);

      // If escape is pressed
      if (ESCAPE_KEYCODE == press->detail) {
        free(event);
        break;
      }
    }

    free(event);
  }

  xcb_destroy_window(xcb.connection, window1);

  // Close connection and free resources
  xcb_disconnect(xcb.connection);
  return 0;
}
