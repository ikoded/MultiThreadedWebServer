# MultiThreadedWebServer

A low level C++ multithreaded webserver for fun. Wanting to learn low level networking and multi threaded request handling.

---

## How To Build/Run

First run `cmake -S . -B build`, and then build with `cmake --build build -j`. This project is confirmed to work on Ubuntu.

---

## Memory Checks and Socket Leaks

This is very important as this can cause very bad errors if not handled properly. With WSL, if configured poorly, it has the capability of crashing your system with a UNEXPECTED_KERNEL_MODE_TRAP.

The first thing I check is `ss -lx | grep /tmp` and `ss -ax | grep /tmp` to make sure no leftover `/tmp/mysocket` (my socket path) exists.

Then I check memory leaks with valgrind: `valgrind --leak-check=full --show-leak-kinds=all ./build/webserver`

Then I use strace to ensure all sockets in program closed properly with: `strace -e trace=open,close,socket,bind,listen -f ./build/webserver`

    Errors are definitely harder to read with this, you need to ensure each socket has a close.

Another one you can use to watch in second terminal as runs is: `watch -n 1 'ps aux | grep webserver; ss -ax | grep mysocket'`

---
