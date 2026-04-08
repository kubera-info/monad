# Input Module

This module processes input data for the application.

Examples of supported input devices include:
* a mouse
* an Xbox controller
* a touch screen

This module works in cooperation with the application window
and is driven by the window’s message queue.

There are two types of cursors used in the Monad(a) Engine:
* Native Windows cursors (**HCURSOR**) — used when the 3D engine is not yet initialized
* Engine-created **3D cursors** — used when the renderer is active

This module also provides a **Screen2Pane** function,
which converts screen-space pixel coordinates into a 3D pane space.