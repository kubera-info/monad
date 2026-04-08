# Wnd Module

This module is designed for working with native windows and displays.
It provides a toolset for two types of windows:
* A single **D3D12** main window
* Multiple secondary windows rendered using **D2D1**

Non-main windows are referred to in the project as **Satellites**.
They use static background images and do not run a game loop.
Only the main window runs the game loop and is always associated
with the primary display, starting at position (0, 0).

This module provides helpers for creating, managing, and destroying windows.
It assumes that each window is created for a **single display**
and uses the same **RECT** dimensions for rendering.