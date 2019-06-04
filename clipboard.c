#include <windows.h>
#include <stdio.h>
#include <string.h>

#define ARR_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))

struct clipboard_format {
  UINT value;
  const char* name;
  const char* description;
};

static struct clipboard_format clipboard_standard_formats[] = {
  { 2, "CF_BITMAP", "A handle to a bitmap (HBITMAP)." },
  { 8, "CF_DIB", "A memory object containing a BITMAPINFO structure followed by the bitmap bits." },
  { 17, "CF_DIBV5", "A memory object containing a BITMAPV5HEADER structure followed by the bitmap color space information and the bitmap bits." },
  { 5, "CF_DIF", "Software Arts' Data Interchange Format." },
  { 0x0082, "CF_DSPBITMAP", "Bitmap display format associated with a private format. The hMem parameter must be a handle to data that can be displayed in bitmap format in lieu of the privately formatted data." },
  { 0x008E, "CF_DSPENHMETAFILE", "Enhanced metafile display format associated with a private format. The hMem parameter must be a handle to data that can be displayed in enhanced metafile format in lieu of the privately formatted data." },
  { 0x0083, "CF_DSPMETAFILEPICT", "Metafile-picture display format associated with a private format. The hMem parameter must be a handle to data that can be displayed in metafile-picture format in lieu of the privately formatted data." },
  { 0x0081, "CF_DSPTEXT", "Text display format associated with a private format. The hMem parameter must be a handle to data that can be displayed in text format in lieu of the privately formatted data." },
  { 14, "CF_ENHMETAFILE", "A handle to an enhanced metafile (HENHMETAFILE)." },
  {
    0x0300,
    "CF_GDIOBJFIRST",
    "Start of a range of integer values for application-defined GDI object clipboard formats. The end of the range is CF_GDIOBJLAST. "
    "Handles associated with clipboard formats in this range are not automatically deleted using the GlobalFree function when the clipboard is emptied. Also, when using values in this range, the hMem parameter is not a handle to a GDI object, but is a handle allocated by the GlobalAlloc function with the GMEM_MOVEABLE flag.",
  },
  { 0x03FF, "CF_GDIOBJLAST", "See CF_GDIOBJFIRST." },
  { 15, "CF_HDROP", "A handle to type HDROP that identifies a list of files. An application can retrieve information about the files by passing the handle to the DragQueryFile function." },
  {
    16,
    "CF_LOCALE",
    "The data is a handle to the locale identifier associated with text in the clipboard. When you close the clipboard, if it contains CF_TEXT data but no CF_LOCALE data, the system automatically sets the CF_LOCALE format to the current input language. You can use the CF_LOCALE format to associate a different locale with the clipboard text. "
    "An application that pastes text from the clipboard can retrieve this format to determine which character set was used to generate the text. "
    "Note that the clipboard does not support plain text in multiple character sets. To achieve this, use a formatted text data type such as RTF instead. "
    "The system uses the code page associated with CF_LOCALE to implicitly convert from CF_TEXT to CF_UNICODETEXT. Therefore, the correct code page table is used for the conversion."
  },
  { 3, "CF_METAFILEPICT", "Handle to a metafile picture format as defined by the METAFILEPICT structure. When passing a CF_METAFILEPICT handle by means of DDE, the application responsible for deleting hMem should also free the metafile referred to by the CF_METAFILEPICT handle." },
  { 7, "CF_OEMTEXT", "Text format containing characters in the OEM character set. Each line ends with a carriage return/linefeed (CR-LF) combination. A null character signals the end of the data." },
  { 0x0080, "CF_OWNERDISPLAY", "Owner-display format. The clipboard owner must display and update the clipboard viewer window, and receive the WM_ASKCBFORMATNAME, WM_HSCROLLCLIPBOARD, WM_PAINTCLIPBOARD, WM_SIZECLIPBOARD, and WM_VSCROLLCLIPBOARD messages. The hMem parameter must be NULL." },
  {
    9,
    "CF_PALETTE",
    "Handle to a color palette. Whenever an application places data in the clipboard that depends on or assumes a color palette, it should place the palette on the clipboard as well. "
    "If the clipboard contains data in the CF_PALETTE (logical color palette) format, the application should use the SelectPalette and RealizePalette functions to realize (compare) any other data in the clipboard against that logical palette. "
    "When displaying clipboard data, the clipboard always uses as its current palette any object on the clipboard that is in the CF_PALETTE format."
  },
  { 10, "CF_PENDATA", "Data for the pen extensions to the Microsoft Windows for Pen Computing." },
  { 0x0200, "CF_PRIVATEFIRST", "Start of a range of integer values for private clipboard formats. The range ends with CF_PRIVATELAST. Handles associated with private clipboard formats are not freed automatically; the clipboard owner must free such handles, typically in response to the WM_DESTROYCLIPBOARD message." },
  { 0x02FF, "CF_PRIVATELAST", "See CF_PRIVATEFIRST." },
  { 11, "CF_RIFF", "Represents audio data more complex than can be represented in a CF_WAVE standard wave format." },
  { 4, "CF_SYLK", "Microsoft Symbolic Link (SYLK) format." },
  { 1, "CF_TEXT", "Text format. Each line ends with a carriage return/linefeed (CR-LF) combination. A null character signals the end of the data. Use this format for ANSI text." },
  { 6, "CF_TIFF", "Tagged-image file format." },
  { 13, "CF_UNICODETEXT", "Unicode text format. Each line ends with a carriage return/linefeed (CR-LF) combination. A null character signals the end of the data." },
  { 12, "CF_WAVE", "Represents audio data in one of the standard wave formats, such as 11 kHz or 22 kHz PCM." }
};

static int displayHelp(int argc, char** argv){
  printf("Usage: %s [OPTION]...\n", argv[0]);
  printf("Get or set the contents of the clipboard\n\n");
  printf("  -l, --list    show all formats available for the clipboard content\n");
  printf("  -c, --copy    copy from the stardand input to the clipboard\n");
  printf("  -p, --paste   paste from the clipboard to the standard output\n");
  printf("  -h, --help    show this help message\n");

  return 0;
}

static int printAllAvailableClipboardFormats(int argc, char** argv){
  OpenClipboard(GetDesktopWindow());

  UINT format = EnumClipboardFormats(0);
  printf("Available clipboard formats:\n\n");

  while(format != 0){
    int found = 0;
    for(int i=0; i<ARR_SIZE(clipboard_standard_formats); i++){
      struct clipboard_format clipFormat = clipboard_standard_formats[i];

      if(clipFormat.value == format){
        printf("%s(0x%x): %s\n\n", clipFormat.name, clipFormat.value, clipFormat.description);
        found = 1;
        break;
      }
    }

    if(!found){
      printf("Non-Standard(0x%x)\n\n", format);
    }

    format = EnumClipboardFormats(format);
  }

  DWORD ret = GetLastError();
  CloseClipboard();
  
  return ret;
}

static int pasteTextClipboard(int argc, char** argv){
  OpenClipboard(GetDesktopWindow());

  if(!IsClipboardFormatAvailable(CF_TEXT)){
    fprintf(stderr, "Only texts can be captured from the clipboard\n");
    return -1;
  }

  HANDLE clipboardHandle = (char*)GetClipboardData(CF_TEXT);
  if(clipboardHandle == INVALID_HANDLE_VALUE){
    fprintf(stderr, "Could not get clipboard handle: Error(0x%x)\n", GetLastError());
    return -1;
  }

  char* clipboardData = (char*)GlobalLock(clipboardHandle);
  printf("%s", clipboardData);

  GlobalUnlock(clipboardHandle);
  CloseClipboard();

  return 0;
}

static int copyTextClipboard(int argc, char** argv){
  OpenClipboard(GetDesktopWindow());

  size_t bufferSize = 1024 * sizeof(char);
  size_t written = 0;
  HGLOBAL streamHandle = GlobalAlloc(GMEM_MOVEABLE, bufferSize);
  if(streamHandle == NULL){
    fprintf(stderr, "Could not allocate memory for the clipboard buffer: Error(0x%x)\n", GetLastError());
    return -1;
  }

  char* streamData = GlobalLock(streamHandle);

  while(!feof(stdin)){
    written += fread(streamData + written, sizeof(char), bufferSize - written, stdin);
    if(bufferSize == written){
      bufferSize *= 2;

      GlobalUnlock(streamHandle);
      streamHandle = GlobalReAlloc(streamHandle, bufferSize, 0);
      streamData = GlobalLock(streamHandle);
    }
  }

  // Alloc extra byte for \0 if needed
  if(bufferSize == written){
    GlobalUnlock(streamHandle);
    streamHandle = GlobalReAlloc(streamHandle, bufferSize + 1, 0);
    streamData = GlobalLock(streamHandle);
  }
  streamData[written] = '\0';

  EmptyClipboard();
  if(SetClipboardData(CF_TEXT, streamHandle) == NULL){
    // You should only free the streamHandle if SetClipboardData failed.
    // If it succeeded, the OS owns the memory we created and will free it
    GlobalFree(streamHandle);

    fprintf(stderr, "Error pasting to the clipboard: Error(0x%x)\n", GetLastError());
    return GetLastError();
  }

  CloseClipboard();
  
  return 0;
}

int main(int argc, char** argv){
  int (* command)(int, char**) = displayHelp;

  if(argc >= 2){
    if(strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
      command = displayHelp;
    } else if(strcmp(argv[1], "-l") == 0 || strcmp(argv[1], "--list") == 0) {
      command = printAllAvailableClipboardFormats;
    } else if(strcmp(argv[1], "-c") == 0 || strcmp(argv[1], "--copy") == 0) {
      command = copyTextClipboard;
    } else if(strcmp(argv[1], "-p") == 0 || strcmp(argv[1], "--paste") == 0) {
      command = pasteTextClipboard;
    }
  }

  return command(argc, argv);
}
