
# don't conserve \n...
curl -X POST  --header "Transfer-Encoding: chunked" -d @subject/webserv.pdf http://127.0.0.1:8080
# conserve \n (and integrity)
curl -X POST  --header "Transfer-Encoding: chunked" --data-binary @sources/Request.cpp http://127.0.0.1:8080
curl -X POST -H "Transfer-Encoding: chunked" -H "Content-Type: application/pdf" --data-binary @subject/webserv.pdf http://127.0.0.1:8080
curl -i -X POST -H "Transfer-Encoding: chunked" --data-binary @tmp/lorem.txt http://127.0.0.1:8080