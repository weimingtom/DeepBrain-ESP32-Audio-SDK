/* 
 * ESPRESSIF MIT License 
 * 
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD> 
 * 
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case, 
 * it is free of charge, to any person obtaining a copy of this software and associated 
 * documentation files (the "Software"), to deal in the Software without restriction, including 
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished 
 * to do so, subject to the following conditions: 
 * 
 * The above copyright notice and this permission notice shall be included in all copies or 
 * substantial portions of the Software. 
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS 
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
 * 
 */

#ifndef _ESP_REQUEST_H_
#define _ESP_REQUEST_H_
#include "req_list.h"
#include "uri_parser.h"
#include "openssl/ssl.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"

#define REQ_BUFFER_LEN  (2048)

#define WS_FIN            0x80
#define WS_OPCODE_TEXT    0x01
#define WS_OPCODE_BINARY  0x02
#define WS_OPCODE_CLOSE   0x08
#define WS_OPCODE_PING    0x09
#define WS_OPCODE_PONG    0x0a
// Second byte
#define WS_MASK           0x80
#define WS_SIZE16         126
#define WS_SIZE64         127
#define MAX_WEBSOCKET_HEADER_SIZE 10
#define WS_RESPONSE_OK    101

typedef enum {
    REQ_SET_METHOD = 0x01,
    REQ_SET_HEADER,
    REQ_SET_HOST,
    REQ_SET_PORT,
    REQ_SET_PATH,
    REQ_SET_URI,
    REQ_SET_SECURITY,
    REQ_SET_POSTFIELDS,
    REQ_SET_DATAFIELDS,
    REQ_SET_DATAFIELDS_PTR,
    REQ_SET_UPLOAD_LEN,
    REQ_SET_PROTOCOL,
    REQ_FUNC_DOWNLOAD_CB,
    REQ_FUNC_UPLOAD_CB,
    REQ_REDIRECT_FOLLOW,
    REQ_FUNC_WEBSOCKET
} REQ_OPTS;

typedef enum {
    PROTOCOL_HTTP = 1,
    PROTOCOL_WEBSOCKET,
    PROTOCOL_SIP
} REQ_PROTOCOL;

typedef enum {
    WS_CONNECTED = 0x01,
    WS_DATA,
    WS_DISCONNECTED,
    WS_ERROR
} REQ_WS_STATUS;

typedef struct response_t {
    req_list_t *header;
    int status_code;
    int len;
} response_t;

typedef struct {
    int bytes_read;
    int bytes_write;
    int bytes_total;
    char *data;
    int at_eof;
} req_buffer_t;

typedef struct request_t {
    req_list_t *opt;
    req_list_t *header;
    SSL_CTX *ctx;
    SSL *ssl;
    req_buffer_t *buffer;
    void *upload_data_ptr; //temp
    int *upload_data_len;
    void *context;
    int socket;
    int (*_connect)(struct request_t *req);
    int (*_read)(struct request_t *req, char *buffer, int len);
    int (*_write)(struct request_t *req, char *buffer, int len);
    int (*_close)(struct request_t *req);
    int (*upload_callback)(struct request_t *req, void *buffer, int len);
    int (*download_callback)(struct request_t *req, void *buffer, int len);
    int (*websocket_callback)(struct request_t *req, int status, void *buffer, int len);
    response_t *response;
    int is_websocket;
    int valid_websocket;
    REQ_PROTOCOL protocol;
} request_t;

typedef int (*download_cb)(request_t *req, void *buffer, int len);
typedef int (*upload_cb)(request_t *req, void *buffer, int len);
// typedef int (*websocket_cb)(request_t *req, int status, void *buffer, int len);

request_t *req_new(const char *url);
void req_setopt(request_t *req, REQ_OPTS opt, void* data);
void req_clean(request_t *req);
void req_close(request_t *req);
int req_write(request_t *req, const char *buffer, int len);
int req_perform(request_t *req);

#endif
