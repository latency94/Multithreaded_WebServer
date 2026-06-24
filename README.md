# Multithreaded HTTP Web Server (C++)

A multithreaded HTTP server built from scratch in C++ using 
POSIX sockets and a custom thread pool. I built this to 
understand how real web servers handle concurrent connections 
at the system level.

## What it does
- Handles multiple client connections simultaneously
- Custom thread pool using std::thread and condition variables
- Parses HTTP/1.1 requests and routes them to handlers
- Serves static HTML/CSS files from disk
- Thread-safe logging with mutex
- /api/status endpoint returns live server stats as JSON

## Tech
C++17, POSIX Sockets, Linux, STL, GNU Make

## Build & Run
make
./server
# open http://localhost:8080

## Routes
GET /            → index.html
GET /hello       → plain text
GET /api/status  → JSON stats
GET /style.css   → stylesheet

## Why I built this
Wanted to understand how threading, sockets, and HTTP work 
together at a low level without hiding behind frameworks.
