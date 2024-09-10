
# Toralizer
A command-line client for connecting to the Tor privacy network.

## Overview

Toralizer is a tool that allows users to route their network traffic through the Tor network by intercepting system calls and redirecting them through a local proxy server. This ensures privacy and anonymity by masking the user's identity when making network requests.

Instead of using standard commands like:

```bash
curl https://12.34.45.87
```

You can prepend `toralize` to the command:

```bash
toralize curl https://12.34.45.87
```

Toralizer will intercept any network calls, replacing the default `connect()` function with our custom implementation, routing traffic through a local proxy connected to the Tor network.

## How it works

1. **Interception of System Calls**: When you run a command that initiates a network connection, Toralizer intercepts the `connect()` system call. This is achieved by replacing the standard `connect()` function with our own custom implementation.
   
2. **Redirection to a Proxy Server**: The tool redirects traffic to a local SOCKS proxy server (which is part of the Tor network). The proxy uses either SOCKS v4 or v5 protocol. The tool handles connecting to this proxy and forwarding requests to the destination.

3. **Shared Library for Interception**: Toralizer uses a shared library (`toralize.so`) that hooks into system calls using the dynamic loader (`dlsym`) functionality. This library is dynamically loaded when the client runs the application.

4. **Network Masking**: The traffic is redirected through the Tor network, effectively masking the user's identity and providing anonymity when making network requests.

## Compilation and Setup

To get started, follow these steps:

### 1. Compile the `toralize` executable:
This command compiles the `toralize` C program:
```bash
gcc toralize.c -o toralize
```

### 2. Create the `toralize.so` shared library:
You need to create the shared library (`toralize.so`) that intercepts the `connect()` system call:
```bash
gcc -shared -o toralize.so -fPIC toralize.c -ldl
```
- `-shared`: Creates a shared object file.
- `-fPIC`: Generates position-independent code, which is necessary for shared libraries.
- `-ldl`: Links the dynamic loader library to handle the loading of symbols (`dlsym`).

### 3. Running the application:
To run the application and intercept network calls using your custom `toralize.so` library, use the `LD_PRELOAD` environment variable:

```bash
LD_PRELOAD=./toralize.so ./toralize curl https://example.com
```

This ensures that any system calls to `connect()` will first be intercepted by the `toralize.so` shared library, routing the traffic through the Tor proxy.

## SOCKS Protocol

The client works with SOCKS v4 and v5 protocols. Below is the structure of the SOCKS v4 request that is used:

```
+----+----+----+----+----+----+----+----+----+----+....+----+
| VN | CD | DSTPORT |      DSTIP        | USERID       |NULL|
+----+----+----+----+----+----+----+----+----+----+....+----+
# of bytes:    1    1      2              4           variable       1
```

- `VN` is the SOCKS version number (4).
- `CD` is the command code (1 for CONNECT).
- `DSTPORT` and `DSTIP` are the destination port and IP address.
- `USERID` is a null-terminated string containing the username.

The SOCKS server processes this request and either grants or rejects the connection.

## Important Notes

- **Shared Libraries**: Shared libraries like `toralize.so` are optimized for modular reuse and efficiency. Instead of embedding code like `printf` or `connect` directly in each program, shared libraries allow common functions to be stored in a central repository and dynamically loaded when needed.
  
  The system also prioritizes function calls based on the load order of shared libraries. When functions with the same name exist (e.g., `connect()`), the function from the most recently loaded shared library is executed.

## License
MIT License

## Helpful Links
Shared libraries == https://rjordaney.is/lectures/hooking_shared_lib/

Socks == https://www.openssh.com/txt/socks4.protocol



