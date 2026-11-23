# Threads Module

This module implements threading support in the Monad(a) Engine.
It provides both **single threads** and **thread pools**.

Each thread may have its own **name**, which does not need to be unique.
For example:
`g_threadNameSidePool = L"Singleton Side Pool"`.

Thread pools execute tasks.
Each thread pool must contain at least one worker thread.

Every structure deriving from **ITask** must specify whether it should be destroyed
after its execution completes.
This is important for supporting reusable tasks.