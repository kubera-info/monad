# Files Module

This module contains various wrappers for Windows API functionality.
It also provides classes for accessing the **Registry** and the **File System**.

The module includes helper utilities as well as a complex **File Manager**.
The File Manager is responsible for loading application assets.
It uses multithreading, allowing the application to continue processing
while files are being loaded.

There are two types of files used in the Monad(a) Engine:
* **Normal files** – such as **.bmp**, **.dds**, **.wav**, etc.
* **Monad(a) files** – files with the **.monad-???** extension

Monad(a) files are encrypted and can optionally be compressed using the **LZMA** algorithm.
They also contain a **SHA-256** checksum.

This module also provides a class for handling simplified XML.
Not all XML features are supported, as only the functionality required by the
**Monad(a) Engine** is implemented.