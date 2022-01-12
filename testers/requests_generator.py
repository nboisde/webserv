import requests as rq
from http.client import HTTPConnection
import os
import time
import json


HTTPConnection._http_vsn_str = "HTTP/1.1"
url = "http://127.0.0.1:8080/chunked"
files = {
	'data' : 'HelloWorld',
	'document' : open(os.path.join('../tmp/lorem.txt'), "rb")
}

headers = {
'Transfer-Encoding': "chunked",
'Accept': "multipart/form-data",
'Content-Type': "application/pdf",
'Cache-Control': "no-cache",
}

def gen():
	yield '6\r\ncoucou\r\n'
	yield '5\r\nmonde\r\n0\r\n\r\n'

i = 0
while i < 100:
	headers = {
'Transfer-Encoding': "chunked",
'Accept': "multipart/form-data",
'Content-Type': "application/pdf",
'Cache-Control': "no-cache",
}
	files = {
	'data' : 'HelloWorld',
	'document' : open(os.path.join('../tmp/lorem.txt'), "rb")
	}
	r = rq.post('http://127.0.0.1:8080/chunked', files=files)#, headers=headers)
	#time.sleep(0.01)
	i+=1

#r = rq.post(url, files=files)
#r = rq.get(url)