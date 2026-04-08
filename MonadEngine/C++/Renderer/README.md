# Renderer Module

This module provides a set of classes for rendering a **3D world** using
**Direct3D 12**, **DXGI**, and related technologies.
Many of these classes encapsulate commonly used rendering code.

The Monad(a) Engine stores the world as a hierarchy of classes derived from a common base.
Each object can be **visible** or **hidden**.
When an object is hidden, all of its descendants are also not rendered.

This module implements simplified usage of **Constant Buffers**,
adapted to the DirectX 12 model.
The application also implements a **matrix stack**, which is a common concept
in game engines.

Usage of **D3D12_INPUT_ELEMENT_DESC** is simplified for common scenarios.

The module also supports **textures** (shader resources),
**custom meshes**, **shaders**, and related rendering objects.

Creating a new application typically starts by copying the provided sample code
and extending it with new functionality.

Many rendering elements are **identified by strings**.
They can be recognized by the **Monad(a) Engine** without direct knowledge
of the application’s internal code.

A typical application uses multiple rendering stages, such as:
**Special**