# Printer Module

This module contains classes used for printing a single page
from a render target using **D2D1**.

It uses a single render target, which is created on demand during the first print operation
and exists until the application terminates.
(The render target is created only when needed.)

The printing process is executed as a task in the thread pool.
The application is expected to wait for the result of the operation.
While printing is in progress, the application may continue processing other tasks,
but it should avoid starting another print job that would use the same render target.

The render target object is declared during 3D world initialization,
however the related classes may not be allocated at that time.

The **Monad(a) Engine** is designed primarily for simplicity and educational use,
therefore printing is supported only to the **default printer** using default settings.

The **printing process may fail**, and in such cases the application can continue running.
For example, the program may play an error jingle and proceed without interruption.