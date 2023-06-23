#pragma once
#include <stdint.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _HTTPHeader {
  char *key;
  char *value;
  struct _HTTPHeader *next;
};
typedef struct _HTTPHeader HTTPHeader;

struct _HTTPRequest {
  const char *action;
  const char *path;
  const char *version;
  const void *payload;
  HTTPHeader *headers;
};
typedef struct _HTTPRequest HTTPRequest;


ssize_t httprequest_read(HTTPRequest *req, int sockfd);
ssize_t httprequest_parse_headers(HTTPRequest *req, char *buffer, ssize_t buffer_len);
const char *httprequest_get_action(HTTPRequest *req);
const char *httprequest_get_header(HTTPRequest *req, const char *key);
const char *httprequest_get_path(HTTPRequest *req);
void httprequest_destroy(HTTPRequest *req);

#ifdef __cplusplus
}
#endif