# Clipboard

Utility for Windows that can paste the contents of the clipboard to stdout or copy the contents from stdint to the clipboard.

## Requirements

  * Windows
  * MSVC Build Tools

## Build

First git clone this repository.

```
git clone https://github.com/neubaner/clipboard.git
```

Then navigate to the project directory and run `build.bat`.

```
cd clipboard
build.bat
```

This will generate an executable called `clipboard.exe` on the same directory.

Finally run it!

```
clipboard
```

### Optional

You can put the `clipboard.exe` on the `PATH` environment variable to use it anywhere on your terminal