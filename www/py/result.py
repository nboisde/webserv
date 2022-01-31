#!/usr/bin/python3
import cgi
import re

form = cgi.FieldStorage()

data = form.getvalue("formule")


print("Content-Type: text/html; charset=utf-8", end="\r\n\r\n")

if data is not None:
    print(data, end='=')

data = data.replace(' ', '')
elem = re.split((r'(\D)'), data)

if elem[1] == '+':
    print(int(elem[0]) + int(elem[2]))
if elem[1] == '-':
    print(int(elem[0]) - int(elem[2]))
if elem[1] == '*':
    print(int(elem[0]) * int(elem[2]))
if elem[1] == '/':
    print(int(elem[0]) / int(elem[2]))