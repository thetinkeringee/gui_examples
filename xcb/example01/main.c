/* 
 * MIT License
 * 
 * Copyright (c) 2024 Ezekiel Holliday 
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xcb/xcb.h>
#include <xcb/xcb_event.h>
#include <xcb/xcb_util.h>
#include <xcb/xproto.h>
#include "util.h"

// Structure to hold xcb specific information
static struct {
  xcb_connection_t *connection;
  int32_t screenNumber;
  xcb_screen_t *screen;

} xcb = {};


// Define the Backgrouund Color
//
// The alpha value will likely have no effect on what displayed without
// other changes to the code due to likely only have 24 bit depth. A later
// example will show how to switch to 32 bit depth.
//
// NOTE: The oreder of the color chanels are a little different than what is
// typically used.
// Chanel order: alpha red green blue
#define BG_COLOR 0xFF404050


int main(void) {

  // This will connect to the default display and screen 0
  xcb.connection = xcb_connect(nullptr, &xcb.screenNumber);

  // Get the screen
  // This function can be repleaced with a for loop iterating over
  // the screens, but this function exists to make it easier.
  xcb.screen = xcb_aux_get_screen(xcb.connection, xcb.screenNumber);

  // Push all commands to the server
  xcb_flush(xcb.connection);

  //  Check to see if a proper connection was possible 
  if (xcb_connection_has_error(xcb.connection)) {
    fprintf(stderr, "Error with connection to X11 server\n");
    return -1;
  }

  // Print some information about your display just because we can
  printf("Your screen is %d x %d pixels\n\n", xcb.screen->width_in_pixels,
         xcb.screen->height_in_pixels);

  //---------------------------------------------------------------------------
  // Creating the Window

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

  // Generate an id for our window
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
                    78,//XCB_WINDOW_CLASS_INPUT_OUTPUT, //
                    xcb.screen->root_visual,       //
                    valueMask, // Specify which values will be pased to server
                    values     // The actual values
  );
  // NOTE: The window size and position may be ignored by some window managers.

  // Give the window a name
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

  // Event loop
    
  xcb_generic_event_t *event = nullptr;

#define ESCAPE_KEYCODE 9

  bool should_exit = false;
  while ((event = xcb_wait_for_event(xcb.connection))) {

    switch (event->response_type & ~80) {

    // Case an xcb error has occured
    case 0: { //Error
        xcb_generic_error_t* error = (xcb_generic_error_t*) event;
    
        const char* const error_type = errorCodeToText(error->error_code);
        const char* const opcode = opcodeToText(error->major_code);

        fprintf(stderr, "XCB %s %s error. Minor opcode %d\n\n", opcode,
                error_type, error->minor_code);
        break;
    }

    // A key press event 
    case XCB_KEY_PRESS: {
      // The event is a key press. Cast the event to a key press event
      xcb_key_press_event_t *press = (xcb_key_press_event_t *)event;

      // print the keycode received
      printf("Keycode: %d\n", press->detail);

      // If escape is pressed
      if (ESCAPE_KEYCODE == press->detail) {
        should_exit = true;
      }
    }

    default: {
      break;
    }

    } // end switch

    free(event);

    if (should_exit) {
      break;
    }
  }

  xcb_destroy_window(xcb.connection, window1);

  // Close connection and free resources
  xcb_disconnect(xcb.connection);
  return 0;
}
