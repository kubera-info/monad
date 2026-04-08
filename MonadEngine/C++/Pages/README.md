# Pages Module

This module implements the page management system.

Each page may contain dialogs, free-rendered elements, and other UI components.
Each **page** has its own **string identifier**. Some pages may be shared between
different applications if their functionality is common.

If a dialog belongs to a specific page, it is not visible when another page is active.
In such a case, nothing inside that dialog is rendered.

Switching between pages is performed using animations.
Some rules are hardcoded, as certain transitions require special handling,
for example when the 3D renderer must be restarted.

An important detail is that changing pages may also affect the 3D cursor,
even while the application state is being updated.