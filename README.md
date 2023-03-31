# AllWrite

This will be a text editor written using C, and OpenGL. 

## v1.0 goals

* Create, edit, save, close and delete files up to 4KiB in size.
* Have working hotkeys, and text area.
* Render using OpenGL calls.
* Display file name, size, location, etc...
* Use a split buffer to store text, no multiline edits
* try to get copy-pasting to work
* No syntax highlighting, or bracket matching.
* Only on linux to start with.

## TODO

* Application Startup and Shutdown 
* Logger
* File I/O
* User Input and Events
* Graphics Abstraction Layer
* UI/UX

## Next Stream TODO

* change result_t and add in fatal messages where appropriate
* change logfile extension to .log, instead of .txt
* setup a testing framework
* make file_manager_open create new file if no file exists
* implement a split buffer. change name of split_buffer_advance to split_buffer_move
* windowing, input
* OpenGL, just OpenGL
