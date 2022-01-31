#!/usr/bin/python3
import cgi
import re

print("Content-Type: text/html; charset=utf-8", end="\r\n\r\n")

print('''<!DOCTYPE html>
<html>
    <body>
    <h1>Calculatrice</h1>
    <form action="calcul.py" method="post">
        <input type="text" name="formule" value="" />
        <input type="submit" value="Calculer">
    </form>
    </body>
</html>''')

form = cgi.FieldStorage()
if form is None:
    exit()

data = form.getvalue("formule")
if data is None:
    exit()
else:

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