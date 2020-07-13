# client-server-filesystem
Simple file client/server application, allows copying, moving, deleting, renaming, downloading and uploading files and directories on server using client GUI app.

Application created using Qt, works for Windows and Linux.

Client can connect to server using IP and port or it can find the server automatically on the host or in LAN.

For Windows platform add 'LIBS += -lws2_32' in client app .pro file to avoid unresolved external symbols errors.

Included Qt .ui files in client app as an example. If you don't want to use Qt, replace .ui and *view* files with appropriate code.
