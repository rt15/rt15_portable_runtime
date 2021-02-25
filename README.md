# rt15 portable runtime (RPR)

It is a thin abstraction layer on top of Linux and Windows.

The concept is similar to Apache Portable Runtime, but with different features and only three platforms (Linux x64, Windows x86 and x64).

It can help to easily:
* Write portable C code (on Linux and Windows) without taking care of implementation details.
* Create Windows programs not (statically or dynamically) linked with the C runtime library (CRT).
* Avoid CRT pitfalls.

It is currently under development so you should not use it yet.

## Write portable C code

When you want to write some code that can compile on Linux and Windows, you quickly face friction because they do not expose the same functions.

For example, you may have to use:
* clock_gettime on Linux and QueryPerformanceCounter on Windows.
* nanosleep on Linux and Sleep on Windows.
* errno on Linux and GetLastError on Windows.
* pthread_mutex_init on Linux and InitializeCriticalSection on Windows.
* pipe2 on Linux and CreatePipe on Windows.
* mkostemp on Linux and GetTempFileName on Windows.
* ...

With RPR you can focus on coding your features, not on wasting your time searching for solutions on the web.

## Create Windows programs without CRT

C programs are almost always either statically or dynamically linked with the CRT.

### Static linking

In case of static linking:
* The resulting binary is big.
* There can be conflicts with the program and its libraries: they can try to use different runtime libraries.
* The operating system cannot re-use the same read-only memory of the CRT in different processes.

### Dynamic linking

In case of dynamic linking, the corresponding dynamic library must exist on the system.
On Linux, libc.so.6 is often assumed to exist.
With Visual Studio, redistributable packages must be installed. Or you can try to link against msvcrt.dll but that is not easy.
On Windows, MinGW links against msvcrt.dll.

### No linking

It is possible to write C programs without linking with the CRT.
Resulting binaries are very small and does not need msvcrt.dll, libc.so.6 or another dynamic library.
It also avoids the CRT execution overhead when the program starts and finishes.

The RPR library supports programs without CRT only under Windows.

## Avoid CRT pitfalls

Many classic C runtime functions are dangerous, for example:
* sprintf, strcat... do not know the buffer size.
* strerror, strtok... are not thread safe.

There are known alternatives but not always available and they can differ between Linux and Windows.

The RPR functions are designed to be safer.
