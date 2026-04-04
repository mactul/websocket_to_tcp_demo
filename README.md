# WebSocket to TCP demo

This is a demo of a realtime socket between a web page and a TCP server.

Everything is in C, no JS is manually written (a bit of JS is automatically written by emscripten). Everything is behind nginx which is in a rootless container, and websockify in another rootless container, providing a good isolation of the components.

## Dependencies installation

```sh
sudo pacman -Syu --needed podman-compose emscripten openbsd-netcat python-pip
pip install -U powermake
```

## Compilation of JS and webassembly files

```sh
python makefile.py -rvi
```

> [!IMPORTANT]  
> The `-i` flag is important as it tells PowerMake to copy the generated .js and .wasm files in the http_public folder.

> [!NOTE]  
> The first compilation will be very slow as emscripten has to build and cache its environment. The following compilations will be fast, even if recompiled entirely with the `-r` flag.

## Starting the environment

First, grant the capability to podman to bind on the port 80
```sh
sudo sysctl -w net.ipv4.ip_unprivileged_port_start=80
```

Then, build containers and run them:
```sh
podman-compose build
podman-compose up
```

You can also run `podman-compose up -d` if you want your services to run in the background.

## Start a listening TCP server on localhost:4000

```sh
nc -l -p 4000
```

## Go on [http://localhost](http://localhost)

If everything is right, on the `nc` server you should see "hello world", you can then type something and press `[enter]` to see the bytes you type beeing received on [http://localhost](http://localhost).

## Once you are done

Once you are done, you can (should) stop all containers with `podman-compose down`.

## Understanding the architecture

### The webpage
The front code of the webpage is in [./http_public/index.html](./http_public/index.html), but the websocket logic is entirely handled by [./src/websocket.c](./src/websocket.c).

### The web server
The index.html and the websocket are served by nginx, which has its config in [./images/nginx/nginx.conf](./images/nginx/nginx.conf).

> [!NOTE]  
> Nginx beeing in a container has only acces to the [./http_public](./http_public/) folder which is mounted at `/var/www/html` (see [./docker-compose.yml](./docker-compose.yml)).

### The websocket to TCP conversion
The websocket to TCP conversion is handled by websockify in a single command that can be found in [./docker-compose.yml](./docker-compose.yml).
