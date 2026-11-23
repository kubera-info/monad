# Kernel Module

This module contains core code shared across multiple engine modules.

It includes, for example, a vector template that does not invoke constructors
for its stored values. This is intentional, as the values may be overwritten
(or assigned multiple times) during their lifetime.

The module is also responsible for issuing internal application events.

It contains the main application prototype code,
which is required to create the main window on the main thread.

Additionally, the module provides reference counters,
helper templates, and other low-level utilities used throughout the engine.