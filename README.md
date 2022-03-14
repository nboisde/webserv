# WEBSERV
### Presentation:
This project is about recoding a server using the HTTP/1.1 protocol.
To do that we used C++ language and Socket/Network programming.
This represent layers 4 to 7 of TCP/IP protocol.
We discovered principles of multiplexing (using poll function for all files descriptors of the programm), but also went deep down into how requests (chunked or not, encoded or not) and responses works (with and without CGIs) by parsing and build them. We also discovered differents HTTP depending of methods since we implemented GET, POST and DELETE.
We also implemented a quite simple static website that shows implemented features directly on the browser.

It was really interesting to implement and discover a protocol that we use everyday without even notice it and understand a little bit deeper how internet works.

### Requirements:
Your system must have clang compiler to create the binary.
If you want launch the website we implemented, you must have installed php-cgi on your machine.

### Compiling the programm:
```make && ./webserv conf/webserv.conf```
This will bind you on port 8080 and port 8181 on the 127.0.0.1 IP.
dont forget to change line ```cgi .php path_of_your_php-cgi``` on conf webserv.conf
visit on your browser http://localhost:8080.

### Credentials:
[Nastasia Demont](https://github.com/ndemont)
[Guillaume Petit](https://github.com/ftputGuigz)
[Nicolas Boisde](https://github.com/nboisde)
