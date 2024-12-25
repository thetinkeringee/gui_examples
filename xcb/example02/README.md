# XCB Example 2: A Properly Closing Window

This example fixes the problem Example 1 has when closing the window by hitting
the x or close button in the upper right hand corner of the window. The issue
is fixed by receiving and handling the WM_DELETE_WINDOW message.

**SEE** [ICCCM WM_DELETE_WINDOW](https://x.org/releases/X11R7.6/doc/xorg-docs/specs/ICCCM/icccm.html#window_deletion)

