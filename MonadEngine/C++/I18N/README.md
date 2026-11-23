# I18N Module

This module provides **internationalization (I18N)** support for the engine and projects.

In some cases, language data may be shared between locales (for example EN_GB and EN_US),
while in others they may differ.

The module provides a template system that can work with various data types:
* **std::wstring**
* **float** — for example, for aligning text differently between languages
* Custom types defined by the developer

It is possible to change the application language at runtime.
The module also provides helpers for **string conversion** and **parameterization**.

Parameterization allows text to be modified using values
that are not known at compile time.