# Audio Module

This module implements a facade over the **XAudio2** API.

Most interactions are performed from the main application thread.
The audio module is thread-safe, as it uses a mutex to protect most external calls.
Time-consuming operations or those requiring critical sections are executed in a dedicated worker thread.
For example, audio files can be converted from **MP3** to **WAV** without blocking the main thread.

The current version of the Monad(a) Engine audio system supports only a single XAudio subsystem.
It is globally accessible through the **Monad::Audio::g_audio** interface.

Each XAudio voice is created lazily — only when it is first needed.
The module architecture consists of layers, some of which are permanent and others dependent on the **XAudio** state.

For example, when no audio output device is available, the permanent (master) layer still exists,
but the XAudio subsystem is not initialized.
In such a case, calls to the audio facade are safely ignored (no-ops), which is considered valid behavior.

Once XAudio becomes available again, the module automatically recreates the entire COM object hierarchy.