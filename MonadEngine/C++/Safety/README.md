# Safety Module

This module is based on the modern Windows **bcrypt** API.

It implements three main operations:
* Calculating **SHA-256** hashes for data buffers
* Encrypting data using **AES-256**
* Decrypting data using **AES-256**

This module is designed to be used together with the **Files** module.
All operations are intended to work on a **Monad(a)** file buffer.

The result of each operation is returned as a container that is more complex
than a standard STL vector, as it also includes additional metadata.