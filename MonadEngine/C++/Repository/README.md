# Repository Module

This module provides an asset repository for the project.

It is a thread-safe subsystem with a simple, unified API.
Each asset must have a **unique name**, which is used to access it.

Each type of asset has its own namespace (name pool).
This means, for example, that a texture and a pixel shader blob
may share the same name, but two pixel shaders cannot.

The module also provides generic repository classes
that can be reused across the application.