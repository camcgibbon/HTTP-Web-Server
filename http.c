#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "http.h"


/**
 * httprequest_parse_headers
 * 
 * Populate a `req` with the contents of `buffer`, returning the number of bytes used from `buf`.
 */

ssize_t httprequest_parse_headers(HTTPRequest *req, char *buffer, ssize_t buffer_len) {
    // Initialize variables
    req->headers = NULL;
    req->payload = NULL;

    // Parse request line
    char *line_end = strchr(buffer, '\r');
    if (!line_end || *(line_end + 1) != '\n') {
        return -1;
    }
    *line_end = '\0';
    char *action = strtok_r(buffer, " ", &buffer);
    char *path = strtok_r(NULL, " ", &buffer);
    char *version = strtok_r(NULL, "\r", &buffer);
    if (!action || !path || !version) {
        return -1;
    }
    req->action = strdup(action);
    req->path = strdup(path);
    req->version = strdup(version);

    // Parse headers
    char *headers_start = line_end + 2;
    char *headers_end = strstr(headers_start, "\r\n\r\n");
    if (!headers_end) {
        return -1;
    }
    *headers_end = '\0'; // End the headers section with null-termination
    char *header_line = strtok_r(headers_start, "\r\n", &buffer);
    while (header_line && *header_line != '\0') {
        char *colon = strchr(header_line, ':');
        if (!colon) {
            return -1;
        }
        *colon = '\0';
        char *key = header_line;
        char *value = colon + 2;
        HTTPHeader *header = (HTTPHeader *)calloc(1, sizeof(HTTPHeader));
        if (!header) {
            return -1;
        }
        header->key = strdup(key);
        header->value = strdup(value);
        header->next = NULL;
        if (!req->headers) {
            req->headers = header;
        } else {
            HTTPHeader *current = req->headers;
            while (current->next) {
                current = current->next;
            }
            current->next = header;
        }
        header_line = strtok_r(NULL, "\r\n", &buffer);
    }

    // Parse payload
    char *payload_start = headers_end + 4; // Start of the payload
    char *content_length_str = httprequest_get_header(req, "Content-Length");
    if (content_length_str) {
        int content_length = atoi(content_length_str);
        if (content_length > 0) {
            req->payload = calloc(content_length + 1, sizeof(char));
            if (!req->payload) {
                return -1;
            }
            memcpy(req->payload, payload_start, content_length);
            ((char *)req->payload)[content_length] = '\0'; // Ensure null-termination
        }
    } else {
        req->payload = strdup(payload_start);
        if (!req->payload) {
            return -1;
        }
    }

    // Return payload size
    if (req->payload) {
        return strlen((char *)req->payload);
    } else {
        return 0;
    }
}


/**
 * httprequest_read
 * 
 * Populate a `req` from the socket `sockfd`, returning the number of bytes read to populate `req`.
 */
ssize_t httprequest_read(HTTPRequest *req, int sockfd) {
    const int BUFFER_SIZE = 5000;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read = 0;
    ssize_t total_bytes_read = 0;
    ssize_t parse_result = 0;

    while ((bytes_read = read(sockfd, buffer + total_bytes_read, BUFFER_SIZE - total_bytes_read)) > 0) {
        total_bytes_read += bytes_read;
        if (total_bytes_read >= BUFFER_SIZE) {
            break;
        }
    }

    if (total_bytes_read > 0) {
        parse_result = httprequest_parse_headers(req, buffer, total_bytes_read);
        if (parse_result == -1) {
            return -1;
        }
    }

    return total_bytes_read;
}


/**
 * httprequest_get_action
 * 
 * Returns the HTTP action verb for a given `req`.
 */
const char *httprequest_get_action(HTTPRequest *req) {
  return req->action;
}


/**
 * httprequest_get_header
 * 
 * Returns the value of the HTTP header `key` for a given `req`.
 */
const char *httprequest_get_header(HTTPRequest *req, const char *key) {
  HTTPHeader *header = req->headers;
  while (header != NULL) {
    if (strcmp(header->key, key) == 0) {
      return header->value;
    }
    header = header->next;
  }
  return NULL;
}


/**
 * httprequest_get_path
 * 
 * Returns the requested path for a given `req`.
 */
const char *httprequest_get_path(HTTPRequest *req) {
  return req->path;
}


/**
 * httprequest_destroy
 * 
 * Destroys a `req`, freeing all associated memory.
 */
void httprequest_destroy(HTTPRequest *req) {
    free(req->action);
    free(req->path);
    free(req->version);

    HTTPHeader *header = req->headers;
    while (header) {
        HTTPHeader *next_header = header->next;
        free(header->key);
        free(header->value);
        free(header);
        header = next_header;
    }
    req->headers = NULL;

    free(req->payload);
    req->payload = NULL;
}