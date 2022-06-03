# core::window

Windowing module

Can create a window and provides an OpenGL context. There is one root window.

Currently for dialog boxes, there is a modal pop up, but once a text module is created dialogs will most likely be within the window.

This documentation will only cover the window API, the specifics are related to the OS/platform so they won't be covered.

Because of the OS specific API, you only get a pointer to a window struct and must pass that around.

## Reference Table

- [Types](#Types)
- [Functions](#Functions)

## Reference 

### Types

#### struct jn_window

The structure for window data, passed around as an opaque pointer.

### Functions

#### jn_window_create

```struct jn_window * jn_window_create(int w, int h)```

Creates a window (specifically the root window)

##### params

###### w

Width of the window

###### h

Height of the window

##### return

A pointer to the ```struct jn_window``` that was created

#### jn_window_destroy

#### jn_window_buffer_swap

#### jn_window_gl_set

#### jn_window_gl_unset

#### jn_window_size_set

#### jn_window_size_get

#### jn_window_dialog