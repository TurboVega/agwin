# <b>agwin</b> - Agon Windows
Manage window-based applications on Agon!

This repository contains an Agon application that displays windows on the screen and allows other windows-based applications to be loaded and executed (either one-shot or stay-resident).

<b>agwin</b> provides a set of commmon "core" functions that may be called by applications. Some of them are intended to be called directly, using pointers from a jump table. Some may be used indirectly via window messages.

<b>agwin</b> is built using AgonDev, from this repository:

https://github.com/AgonPlatform/agondev/tree/master


## Essential Theory

### Window Tree

<b>agwin</b> maintains a tree (hierarchy) of windows, starting with the <i>root window</i>, which it creates during startup. All other windows are descendents of the root window.

The window structure contains many fields. Here are the ones related to the tree of windows:

```
typedef struct tag_AwWindow {
    AwWindow*       parent;         // points to the parent window
    AwWindow*       first_child;    // points to the first child window
    AwWindow*       last_child;     // points to the last child window
    AwWindow*       prev_sibling;   // points to the previous sibling window
    AwWindow*       next_sibling;   // points to the next sibling window
} AwWindow;

```

In terms of the display, for any particular window (<i>W</i>), the top-down visibility (layering) precedence is as follows:

* Highest precedence (front-most): first child window of <i>W</i>, if any (<i>first_child</i>).
* Middle precedence (just behind the front): the window <i>W</i> itself.
* Lowest precedence (rear-most): next sibling window of <i>W</i>, if any (<i>next_sibling</i>).

Windows are displayed by walking the window tree recursively, starting at the root window. Because of the precedence rules, the children of any window may partially or fully cover the area of that window. The window will cover its siblings (those having the same parent as the window), if it overlaps them.

<u>Do not assume</u> that the <i>first_child</i> points to the oldest child. It does not. New windows are inserted at the <i>head</i> of the list, rather than at the <i>tail</i>, causing <i>first_child</i> to point to the newest child. Thus, when child windows A, B, and C are created in that order, if they happen to overlap, the C will occlude B, and B will occlude A, because C was the <i>last</i> window created, and appears as the <i>first</i> window in the linked list. In short, <i>first_child</i> and <i>last_child</i> refer to items in the list, not to the timing of window creation.

### Screen Coordinates

<b>agwin</b> uses device coordinates for all of its operations, not logical coordinates, so it always turns off logical coordinate usage when establishing the viewports for drawing parts of windows.

In <b>agwin</b>, rectangles <i>used to manage windows</i> always use a typical computer graphics format, where the upper-left X coordinate (called <i>left</i>) is less than or equal to the lower-right X coordinate (called <i>right</i>), and the upper-left Y coordinate (called <i>top</i>) is less than or equal to the lower-right Y coordinate (called <i>bottom</i>). The upper-left coordinate is <i>included</i> within the rectangle, but the lower-right coordinate is <i>excluded</i>. With this format, it is easy to compute the width of the rectangle as <i>right-left</i>, and the height of the rectangle as <i>bottom-top</i>. Note that <b>agwin</b> automatically adjusts the lower-right coordinates, subtracting one (1) when establishing a viewport, because the VDP uses an <i>inclusive</i> endpoint.

The simplest empty rectangle (width=0 and height=0) has all 4 values as zeros. In fact, the "core_rect_is_empty()" function returns <i>true</i> if either the width or height is zero.

### Message Processing

The <b>agwin</b> main loop is little more than a message queue processor, with some extra checks for things like the keyboard, mouse, real-time clock, and a simple timer.

Events are posted to the message queue, and may be received by the <i>message handlers</i> of interested windows. A window shows interest in real-time clock and/or simple timer events by having a <i>window style</i> that has the corresponding flags set. A window is automatically interested in keyboard and mouse events when it is the currently <i>active</i> window.

<b>agwin</b> posts various messages to the queue, plus when required, it processes some messages directly, bypassing the queue. Processing a message means invoking the proper message handler function, giving the owning application a chance to do something with a window message. That invocation takes place on the <b>agwin</b> stack, not on any stack defined by the invoked application. More on that in the next section...

Message processing centers around the concept of <i>window classes</i>. A <i>class</i> in this case refers to one or more windows that have their window messages processed by a particular window message handler (function). Note that a window class may have a parent class, and when processing a message, <b>agwin</b> will start with the handler specific to the window, then move through each parent handler (based on the class hierarchy), unless it is told to <i>halt</i> processing for the message.

Parent versus child classes may be used to modify/enhance the message processing. For example, if a certain class is a child of the "edit box" class, it may provide specific handling of certain window messages, and pass those or other messages along to the "edit box" class, for further handling. Since the class pointers are easily accessible, the custom class function could invoke the parent class first, then add its own processing, and then halt further processing of the message.

### Application Loading

<b>agwin</b> allows other windows applications to be loaded and executed, meaning that after pulling an application into RAM (from SD card), <b>agwin</b> will call the <i>main(argc,argv)</i> function of the application. In doing so, <b>agwin</b> passes the name of the application as the first argument (typical in C), and a pointer to the <b>agwin</b> core function table (in ASCII hexadecimal) as the second argument. The application may keep a local copy of that pointer, so that it can call some core functions directly.

When an application is loaded, and <b>agwin</b> invokes its <i>main()</i> function, <b>agwin</b> expects that the function will do some initialization and return, because <b>agwin</b> runs a <i>cooperative multitasking</i> system on a single stack. Because the loaded application is a normal MOS application, it will create its own stack; however, that stack will only be useful <i>during</i> the <i>main()</i> function. For that reason, the loaded application should be compiled in such a manner as to keep its own stack as small as possible, to avoid wasting memory that might be useful for other additional applications.

The loaded application may do all of its work, and finish by exiting <i>main()</i>, or it may create one or more windows, and indicate that it should remain resident. This indication comes via the return value from <i>main()</i>. If that value ranges from just past the <b>agwin</b> memory area to just below the area of memory reserved by MOS, then the value is a pointer to the list of <i>window classes</i> defined by the loaded application, and <b>agwin</b> expects that the application will remain resident, and remembers it in its own table of loaded applications. This makes it possible for <b>agwin</b> to kill any windows handled by that application when it is unloaded. If the <i>main()</i> return value is a number close to zero, as is typical for console error codes, then <b>agwin</b> does not record the application, and assumes that it will not care to remain resident.

### Window Drawing


## Suggested Enhancements

* Add core functions to lookup window classes by name, especially for any new classes defined by <b>agwin</b> itself, such as the "edit box" (which is suggested, but not implemented).
* Add code and windows messages to support common widgets, such as edit box, menu, list box, and icon (bitmap).
* Add code and windows messages to support a desktop. A desktop may be created in either of two ways: (1) as a separate application that is loaded, or (2) by using the root window pointer to manipulate the root window.
* Modify the window structure so that it can reference an icon for the window.
* Determine an approach for representing an icon bitmap for an application or for a window within an application. This could be something as simple as a file-naming convention (application versus bitmap file), or something more complex, involving the executable file format.
* Create a file explorer application.
* Create a text editor application.
* Create a terminal-like application, intended to run console applications (those that do not have their own custom windows).
* Modify argument passing to a loaded application so that user-defined arguments may be given to that application when loading it.
* Make a PC tool that can arrange the load addresses and memory sizes of multiple windows applications, so that they may be resident simultaneously without clobbering each other. Alternatively, find a good way to compile relocatable code for the EZ80!

## Sample Agon Windows App

Show a clock in a window:
https://github.com/TurboVega/winclock

