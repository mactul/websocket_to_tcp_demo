#include <emscripten/emscripten.h>
#include <emscripten/websocket.h>
#include <emscripten/html5.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

EM_JS(bool, element_by_id_set_inner_text, (const char* id, const char* text), {
    const elem = document.getElementById(UTF8ToString(id));
    if(!elem)
    {
        return false;
    }
    elem.innerText = UTF8ToString(text);
    return true;
});

EM_BOOL onopen(int eventType, const EmscriptenWebSocketOpenEvent* websocketEvent, void* userData)
{
    printf("WebSocket opened!\n");

    EMSCRIPTEN_WEBSOCKET_T socket = (EMSCRIPTEN_WEBSOCKET_T)(intptr_t)userData;

    const char* message = "hello world\n";

    // Send as binary
    // This is important as websockify will not be able to understand text websockets.
    EMSCRIPTEN_RESULT result = emscripten_websocket_send_binary(socket, (void*)message, strlen(message));

    if(result == EMSCRIPTEN_RESULT_SUCCESS)
    {
        printf("Sent '%s' as binary data\n", message);
    }
    else
    {
        printf("Failed to send message, error code: %d\n", result);
    }

    return EM_TRUE;
}

EM_BOOL onmessage(int eventType, const EmscriptenWebSocketMessageEvent* websocketEvent, void* userData)
{
    char message_buf[256];

    printf("Received message:\n");
    printf("  numBytes: %d\n", websocketEvent->numBytes);
    printf("  isText: %d\n", websocketEvent->isText);

    if(websocketEvent->isText)
    {
        printf("  text data: %s\n", (char*)websocketEvent->data);
    }
    else
    {
        printf("  binary data (hex): ");
        size_t j = 0;
        for(uint32_t i = 0; i < websocketEvent->numBytes; i++)
        {
            printf("%02X ", websocketEvent->data[i]);
            if(j < sizeof(message_buf) - 1 && isascii(((char*)websocketEvent->data)[i]))
            {
                message_buf[j] = ((char*)websocketEvent->data)[i];
                j++;
            }
        }
        message_buf[j] = '\0';
        printf("\n");
    }

    if(!element_by_id_set_inner_text("received", message_buf))
    {
        printf("Error, unable to modify element \"received\"\n");
    }

    return EM_TRUE;
}

EM_BOOL onerror(int eventType, const EmscriptenWebSocketErrorEvent* websocketEvent, void* userData)
{
    printf("WebSocket error!\n");
    return EM_TRUE;
}

EM_BOOL onclose(int eventType, const EmscriptenWebSocketCloseEvent* websocketEvent, void* userData)
{
    printf("WebSocket closed:\n");
    printf("  wasClean: %d\n", websocketEvent->wasClean);
    printf("  code: %d\n", websocketEvent->code);
    printf("  reason: %s\n", websocketEvent->reason);
    return EM_TRUE;
}

int main()
{
    printf("Starting WebSocket connection...\n");

    if(!emscripten_websocket_is_supported())
    {
        printf("WebSockets are not supported!\n");
        return 1;
    }

    EmscriptenWebSocketCreateAttributes ws_attrs = {
        "ws://localhost/ws",
        NULL,
        EM_TRUE
    };

    EMSCRIPTEN_WEBSOCKET_T socket = emscripten_websocket_new(&ws_attrs);

    if(socket <= 0)
    {
        printf("Failed to create WebSocket (error code: %d)\n", socket);
        return 1;
    }

    printf("WebSocket created, connecting...\n");

    emscripten_websocket_set_onopen_callback(socket, (void*)(intptr_t)socket, onopen);
    emscripten_websocket_set_onmessage_callback(socket, (void*)(intptr_t)socket, onmessage);
    emscripten_websocket_set_onerror_callback(socket, (void*)(intptr_t)socket, onerror);
    emscripten_websocket_set_onclose_callback(socket, (void*)(intptr_t)socket, onclose);

    printf("Callbacks registered. Connection will be established asynchronously.\n");
    printf("Open the browser console to see the output.\n");

    return 0;
}
