## What?

CTodos is a Todos List web app, written in pure c. It only has a dependency on the c standard library.

It has been tested on linux, although it should work on other posix operating systems.

It is a stateful application, and does not use a database, as one of the constraints for this web app
was zero libraries.

It also does not have the concept of users, and everyone sees and modifies the same todo list.


## How?

The sockets api in the C standard library allows you to create a TCP server.

From there, we can create a simple http implementation, and build a web application

It extensively uses `sprintf` for templating behind the scenes

## Where to run?

Due to security concerns, this application won't be publicly hosted. Instead, run the docker container locally.

The application has a compile-time dependency on gnu autotools (autoconf and automake), but has no runtime dependencies (other then the libc)

The application will run on port 8080 at ipv6 `[::1]`. By passing `--disable-ipv6` to the `./configure` script, you can make it always use and redirect you to ipv4. Currently, the application will always make redirects to `[::1]:8080` on ipv6 builds and `localhost:8080` on ipv4 builds. 

To make it redirect to something else, edit the `todo_page_handler` in `src/router.c`, and replace the lines 
```c
sprintf(res + strlen(res), "Location: http://[::1]:%d/\r\n",
		SERVER_PORT);
```
and
```c
sprintf(res + strlen(res), "Location: http://localhost:%d/\r\n",
		SERVER_PORT);
```
with your preferred redirect link. It should redirect you to the index page 

## Build Instructions

First, install gnu autotools and a c compiler.

Then run the following commands in the project directory
```bash
cd /path/to/project/
autoreconf --install .
./configure
make all
```

The application binary should be compiled to `src/ctodos`.

## Docker builds

To build a docker image, run the following command in the project directory
```bash
docker build . --tag="ctodos"
```

and then run the application using:
```bash
docker run --rm -p 8080:8080 ctodos
```