#!/bin/python3

import websocket

ws = websocket.WebSocket()

ws.connect("ws://localhost/ws")

ws.send_binary(b"Hello World\n")

print(ws.recv())
