# HTTP-Web-Server
Web server that handles incoming connections, parses request headers and content, and sends appropriate responses, providing a robust foundation for web-based applications

## Structure

- **[server.c]** Main server file where the server is initialized, and client requests are handled.
- **[http.c] and [http.h]** Contains functionalities for handling and processing HTTP requests.
- **[Makefile]** Used for easy compilation and building of the project.
- **[Dockerfile]** Docker configuration file for containerizing the application.

## Compilation

You can compile the project using the provided Makefile:

```bash
make
```
## Running the Server

After compilation, you can run the server using the following command:

```bash
./server
```
## Docker Support

You can build and run the application using Docker. Use the following commands:

```bash
docker build -t http-web-server .
docker run http-web-server
```
