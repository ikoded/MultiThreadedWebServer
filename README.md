# MultiThreadedWebServer

A low level C++ multithreaded webserver for fun. Wanting to learn low level networking and multi threaded request handling. It has a menu at beginning to transfer over a Unix Domain or a TCP domain. I also added for TCP serving an HTML in the data to `localhost:8080`, this is to learn differences of HTML serving as well as make server more flexible

---

## How To Build/Run

First run `cmake -S . -B build`, and then build with `cmake --build build -j`, finally run `./build/webserver`. This project is confirmed to work on Ubuntu with WSL.

---

## Memory Checks and Socket Leaks

This is very important as this can cause very bad errors if not handled properly. With WSL, if configured poorly, it has the capability of crashing your system with a UNEXPECTED_KERNEL_MODE_TRAP.

- The first thing I check is `ss -lx | grep /tmp` and `ss -ax | grep /tmp` to make sure no leftover `/tmp/mysocket` exists (for UNIX Domain specifically). I check `ss -ltnp | grep ':8080'` & `lsof -nP -iTCP:8080` to make sure the port 8080 is cleaned up and not owned (for TCP Domain specifically).

- Then I check memory leaks with valgrind: `valgrind --leak-check=full --show-leak-kinds=all ./build/webserver`

- Then I use strace to ensure all sockets in program closed properly with: `strace -e trace=open,close,socket,bind,listen -f ./build/webserver` or `strace -f -e trace=network,close ./build/webserver` for connect/send/recv.
  - Errors are definitely hard to read with this, you just need to ensure each socket has a close.

- Another one you can use to watch in second terminal as runs is: `watch -n 1 'ps aux | grep webserver; ss -ax | grep mysocket'`

---

## Future Development

### Cuda Programming

Would like to bring my GPU in the mix to do things such as add a Gaussian Filter to an image pixel by pixel (needs to be large image to be worth it). This server can certainly do that now that both Domains are setup properly. I have made a correctly setup Client Server model.

### HTML Server SIGTERM Handling

While not fully necessary, would be nice to cleanup resources on SIGTERM (when user presses Ctrl+C).