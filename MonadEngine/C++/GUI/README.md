# GUI Module

This module contains basic classes for **dialogs** and **controls**.
It implements a GUI subsystem written entirely from scratch.

Each dialog consists of a **border**, **header**, **horizontal separator**, and **body**.
Dialogs can use fonts and emojis, mixing various visual styles.

There are several types of buttons:
* **Iconic Push Button**
* **Iconic Radio Button** – radio buttons are always managed by a radio-button group
* **Iconic Checkbox**

Each button has its own color and monochrome icon.
There are also language-selection buttons that use national flags and are not monochrome.

Buttons use dedicated **look-and-feel** classes.
It is possible to have multiple buttons with the same visual style,
for example an “Exit” button used in different dialogs.
Each button can optionally play a random jingle when clicked.

The module also provides a class representing the **background** of the renderer’s screen.
It always covers the entire 3D window and does not preserve its own aspect ratio.

Buttons can be **visible or hidden**, and **enabled or disabled**.
Two-state buttons use two separate icons — one for each state.
**Two-state buttons** may also have **labels** displayed to their right.