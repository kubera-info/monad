# System Module

This module provides functionality related to the **operating system**.

It includes a **DLL loader** with a simple function invocation interface.

The module also provides functionality for setting the application's exit code.
By default, the exit code is **ERROR_SUCCESS**, but another error code may be set once.
Any further attempts to change the exit code are ignored.

Additional low-level system-related utilities are also included in this module.