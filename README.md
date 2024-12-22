# XCB and Wayland client example 

This is a growing collection of XCB and Wayland client examples. Initially
everything will be in C, but I'm hoping to add some other languages too. I may
even add some WIN32 stuff just becasue doing things from scratch like this is a
fun.

- Example 01 
 
  Basic example of creating a window with xcb. The window is closed by pressing
  the escape key

- Example 02

  This xcb example is an extension of example 01 that fixes the problem of
  clicking on the X or close button in the top right corner of the window. This
  is accomplished by handling the WM_DELTE_WINDOW message and properly exiting.

- Example 03

  This xcb example modifies example 02 to be a fixed size window by by setting
  the WM_NORMAL_HINTS property.

- Example 04

  This xcb example shows how to make the window's depth 32 bits and 
  background transparent.

---

## Requests

If you have and example you would like to see, submit an issue and maybe I'll 
see what I can do.


