# AllWrite

This will be a text editor written using C, and OpenGL.

## Dependencies

This software depends on a number of libraries. The following libraries have their licenses included in the docs folder under folders with the following names:

- Freetype2 - the Freetype library is being used here for truetype font rasterization.
- GLEW - The OpenGL Extension Wrangler is being used to load OpenGL functions.
- GLFW - GLFW is being used to create a window and get input from the X server.
- stb - stb_image is being used to load textures.

## v1.0 goals

- Create, edit, save, close and delete files up to 4KiB in size. - Implemented, except for delete
- Have working hotkeys, and text area. - Implemented
- Render using OpenGL calls. - Implemented
- Display file name, size, location, etc... - Implemented name and location, but not size.
- Use a split buffer to store text, no multiline edits - Implemented
- try to get copy-pasting to work - Not implemented
- Only on linux to start with.
