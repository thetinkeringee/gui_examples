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
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
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
#include "util.h"

static const struct {
  union {
    struct {
      const char *Request;
      const char *Value;
      const char *Window;
      const char *Pixmap;
      const char *Atom;
      const char *Cursor;
      const char *Font;
      const char *Match;
      const char *Drawable;
      const char *Access;
      const char *Alloc;
      const char *Colormap;
      const char *GContext;
      const char *IDChoice;
      const char *Name;
      const char *Length;
      const char *Implementation;
    } name;
    const char *code[17];
  };
} error_codes= {.name = {
                    .Request = "Request",
                    .Value = "Value",
                    .Window = "Window",
                    .Pixmap = "Pixmap",
                    .Atom = "Atom",
                    .Cursor = "Cursor",
                    .Font = "Font",
                    .Match = "Match",
                    .Drawable = "Drawable",
                    .Access = "Access",
                    .Alloc = "Alloc",
                    .Colormap = "Colormap",
                    .GContext = "GContext",
                    .IDChoice = "IDChoice",
                    .Name = "Name",
                    .Length = "Length",
                    .Implementation = "Implementation",
                }};

const char *const error_unknown = "Unknown";

// This function converts and error code to a readable
// error type.
const char *errorCodeToText(uint8_t error_code) {

  if (error_code >=1 && error_code <= 17) {
        return error_codes.code[error_code];
  }

  return error_unknown;
}

static const struct {
  union {
    struct {
      const char *CreateWindow;
      const char *ChangeWindowAttributes;
      const char *GetWindowAttributes;
      const char *DestroyWindow;
      const char *DestroySubwindows;
      const char *ChangeSaveSet;
      const char *ReparentWindow;
      const char *MapWindow;
      const char *MapSubWindows;
      const char *UnmapWindow;
      const char *UnmapSubWindows;
      const char *ConfigureWindow;
      const char *CirculateWindow;
      const char *GetGeometry;
      const char *QueryTree;
      const char *InternAtom;
      const char *GetAtomName;
      const char *ChangeProperty;
      const char *DeleteProperty;
      const char *GetProperty;
      const char *ListProperties;
      const char *SetSelectionOwner;
      const char *GetSelectionOwner;
      const char *ConvertSelection;
      const char *SendEvent;
      const char *GrabPointer;
      const char *UngrabPointer;
      const char *GrabButton;
      const char *UngrabButton;
      const char *ChangeActivePointerGrab;
      const char *GrabKeyboard;
      const char *UngrabKeyboard;
      const char *GrabKey;
      const char *UngrabKey;
      const char *AllowEvents;
      const char *GrabServer;
      const char *UngrabServer;
      const char *QueryPointer;
      const char *GetMotionEvents;
      const char *TranslateCoordinates;
      const char *WarpPointer;
      const char *SetInputFocus;
      const char *GetInputFocus;
      const char *QueryKeymap;
      const char *OpenFont;
      const char *CloseFont;
      const char *QueryFont;
      const char *QueryTextExtents;
      const char *ListFonts;
      const char *ListFontsWithInfo;
      const char *SetFontPath;
      const char *GetFontPath;
      const char *CreatePixmap;
      const char *FreePixmap;
      const char *CreateGC;
      const char *ChangeGC;
      const char *CopyGC;
      const char *SetDashes;
      const char *SetClipRectangles;
      const char *FreeGC;
      const char *ClearArea;
      const char *CopyArea;
      const char *CopyPlane;
      const char *PolyPoint;
      const char *PolyLine;
      const char *PolySegment;
      const char *PolyRectangle;
      const char *PolyArc;
      const char *FillPoly;
      const char *PolyFillRectangle;
      const char *PolyFillArc;
      const char *PutImage;
      const char *GetImage;
      const char *PolyText8;
      const char *PolyText16;
      const char *ImageText8;
      const char *ImageText16;
      const char *CreateColormap;
      const char *FreeColormap;
      const char *CopyColormapAndFree;
      const char *InstallColormap;
      const char *UninstallColormap;
      const char *ListInstalledColormaps;
      const char *AllocColor;
      const char *AllocNamedColor;
      const char *AllocColorCells;
      const char *AllocColorPlanes;
      const char *FreeColors;
      const char *StoreColors;
      const char *StoreNamedColors;
      const char *QueryColors;
      const char *LookupColor;
      const char *CreateCursor;
      const char *CreateClyphCursor;
      const char *FreeCursor;
      const char *RecolorCursor;
      const char *QueryBestSize;
      const char *QueryExtension;
      const char *ListExtensions;
      const char *ChangeKeyboardMapping;
      const char *GetKeyboarMapping;
      const char *ChangeKeyboardControl;
      const char *GetKeyboardControl;
      const char *Bell;
      const char *ChangePointerControl;
      const char *GetPointerControl;
      const char *SetScreenSaver;
      const char *GetScreenSaver;
      const char *ChangeHosts;
      const char *ListHosts;
      const char *SetAccessControl;
      const char *SetCloseDownMode;
      const char *KillClient;
      const char *RotateProperties;
      const char *ForceScreenSaver;
      const char *SetPointerMapping;
      const char *GetPointerMapping;
      const char *SetModifierMapping;
      const char *GetModifierMapping;
      const char *NoOperation;
    } name;
    const char *const code[120];
  };
} opcodes = { //
    .name = {
        .CreateWindow = "CreateWindow",
        .ChangeWindowAttributes = "ChangeWindowAttributes",
        .GetWindowAttributes = "GetWindowAttributes",
        .DestroyWindow = "DestroyWindow",
        .DestroySubwindows = "DestroySubwindows",
        .ChangeSaveSet = "ChangeSaveSet",
        .ReparentWindow = "ReparentWindow",
        .MapWindow = "MapWindow",
        .MapSubWindows = "MapSubWindows",
        .UnmapWindow = "UnmapWindow",
        .UnmapSubWindows = "UnmapSubWindows",
        .ConfigureWindow = "ConfigureWindow",
        .CirculateWindow = "CirculateWindow",
        .GetGeometry = "GetGeometry",
        .QueryTree = "QueryTree",
        .InternAtom = "InternAtom",
        .GetAtomName = "GetAtomName",
        .ChangeProperty = "ChangeProperty",
        .DeleteProperty = "DeleteProperty",
        .GetProperty = "GetProperty",
        .ListProperties = "ListProperties",
        .SetSelectionOwner = "SetSelectionOwner",
        .GetSelectionOwner = "GetSelectionOwner",
        .ConvertSelection = "ConvertSelection",
        .SendEvent = "SendEvent",
        .GrabPointer = "GrabPointer",
        .UngrabPointer = "UngrabPointer",
        .GrabButton = "GrabButton",
        .UngrabButton = "UngrabButton",
        .ChangeActivePointerGrab = "ChangeActivePointerGrab",
        .GrabKeyboard = "GrabKeyboard",
        .UngrabKeyboard = "UngrabKeyboard",
        .GrabKey = "GrabKey",
        .UngrabKey = "UngrabKey",
        .AllowEvents = "AllowEvents",
        .GrabServer = "GrabServer",
        .UngrabServer = "UngrabServer",
        .QueryPointer = "QueryPointer",
        .GetMotionEvents = "GetMotionEvents",
        .TranslateCoordinates = "TranslateCoordinates",
        .WarpPointer = "WarpPointer",
        .SetInputFocus = "SetInputFocus",
        .GetInputFocus = "GetInputFocus",
        .QueryKeymap = "QueryKeymap",
        .OpenFont = "OpenFont",
        .CloseFont = "CloseFont",
        .QueryFont = "QueryFont",
        .QueryTextExtents = "QueryTextExtents",
        .ListFonts = "ListFonts",
        .ListFontsWithInfo = "ListFontsWithInfo",
        .SetFontPath = "SetFontPath",
        .GetFontPath = "GetFontPath",
        .CreatePixmap = "CreatePixmap",
        .FreePixmap = "FreePixmap",
        .CreateGC = "CreateGC",
        .ChangeGC = "ChangeGC",
        .CopyGC = "CopyGC",
        .SetDashes = "SetDashes",
        .SetClipRectangles = "SetClipRectangles",
        .FreeGC = "ClearArea",
        .ClearArea = "ClearArea",
        .CopyArea = "CopyArea",
        .CopyPlane = "CopyPlane",
        .PolyPoint = "PolyPoint",
        .PolyLine = "PolyLine",
        .PolySegment = "PolySegment",
        .PolyRectangle = "PolyRectangle",
        .PolyArc = "PolyArc",
        .FillPoly = "FillPoly",
        .PolyFillRectangle = "PolyFillRectangle",
        .PolyFillArc = "PolyFillArc",
        .PutImage = "PutImage",
        .GetImage = "GetImage",
        .PolyText8 = "PolyText8",
        .PolyText16 = "PolyText16",
        .ImageText8 = "ImageText8",
        .ImageText16 = "ImageText16",
        .CreateColormap = "CreateColormap",
        .FreeColormap = "FreeColormap",
        .CopyColormapAndFree = "CopyColormapAndFree",
        .InstallColormap = "InstallColormap",
        .UninstallColormap = "UninstallColormap",
        .ListInstalledColormaps = "ListInstalledColormaps",
        .AllocColor = "AllocColor",
        .AllocNamedColor = "AllocNamedColor",
        .AllocColorCells = "AllocColorCells",
        .AllocColorPlanes = "AllocColorPlanes",
        .FreeColors = "FreeColors",
        .StoreColors = "StoreColors",
        .StoreNamedColors = "StoreNamedColors",
        .QueryColors = "QueryColors",
        .LookupColor = "LookupColor",
        .CreateCursor = "CreateCursor",
        .CreateClyphCursor = "CreateClyphCursor",
        .FreeCursor = "FreeCursor",
        .RecolorCursor = "RecolorCursor",
        .QueryBestSize = "QueryBestSize",
        .QueryExtension = "QueryExtension",
        .ListExtensions = "ListExtensions",
        .ChangeKeyboardMapping = "ChangeKeyboardMapping",
        .GetKeyboarMapping = "GetKeyboarMapping",
        .ChangeKeyboardControl = "ChangeKeyboardControl",
        .GetKeyboardControl = "GetKeyboardControl",
        .Bell = "Bell",
        .ChangePointerControl = "ChangePointerControl",
        .GetPointerControl = "GetPointerControl",
        .SetScreenSaver = "SetScreenSaver",
        .GetScreenSaver = "GetScreenSaver",
        .ChangeHosts = "ChangeHosts",
        .ListHosts = "ListHosts",
        .SetAccessControl = "SetAccessControl",
        .SetCloseDownMode = "SetCloseDownMode",
        .KillClient = "KillClient",
        .RotateProperties = "RotateProperties",
        .ForceScreenSaver = "ForceScreenSaver",
        .SetPointerMapping = "SetPointerMapping",
        .GetPointerMapping = "GetPointerMapping",
        .SetModifierMapping = "SetModifierMapping",
        .GetModifierMapping = "GetModifierMapping",
        .NoOperation = "NoOperation",
    }};

static const char *const unknownOpcode = "Uknown Opcode";
static constexpr int nOpcodes = sizeof(opcodes.name) / sizeof(const char *);
static_assert(nOpcodes == 120);

const char *opcodeToText(uint8_t opcode) {
  if (opcode >= 1 && opcode <= nOpcodes) {
    return opcodes.code[opcode - 1];
  }
  return unknownOpcode;
}
