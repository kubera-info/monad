# Exceptions Module

This module contains runtime exception handling and reporting tools.

The **Monad(a) Engine** provides its own base class for C++ exceptions,
but it also supports standard **STD exceptions**.

Another feature of this module is an **Event Viewer** wrapper for simplified usage.

Only one exception can be thrown to stop the application,
and only one error can be reported at a time.

Each application can define its own error-handling strategy.
For example, one application may display a **MessageBox()**,
while another may invoke an external error-reporting tool.

One important note: this module provides a macro for catching exceptions.
The exception is not processed inside a `try` block,
because all handling logic is performed inside the exception constructor itself.