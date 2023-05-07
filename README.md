# AllWrite

This will be a text editor written using C, and OpenGL.

## Dependencies

This software depends on a number of libraries. The following libraries have their licenses included in the docs folder under folders with the following names:

- Freetype2 - the Freetype library is being used here for truetype font rasterization.
- GLEW - The OpenGL Extension Wrangler is being used to load OpenGL functions.
- GLFW - GLFW is being used to create a window and get input from the X server.
- stb - stb_image is being used to load textures.

## v1.0 goals

- Create, edit, save, close and delete files up to 4KiB in size.
- Have working hotkeys, and text area.
- Render using OpenGL calls.
- Display file name, size, location, etc...
- Use a split buffer to store text, no multiline edits
- try to get copy-pasting to work
- No syntax highlighting, or bracket matching.
- Only on linux to start with.

## TODO

- Application Startup and Shutdown
- Logger
- File I/O
- User Input and Events
- Graphics Abstraction Layer
- UI/UX

## Next Stream TODO

- Explain fixes made to font primitive
- Finish font primitive
- display file contents
- display file name

## Keybindings

- Movement - alt + [i,j,k,l]
- Open File - ctrl + o
- Close File - ctrl + q
- Save File - ctrl + s
- Exit program - esc
