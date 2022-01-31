#!/usr/bin/python3
import cgi

print("Content-Type: text/html; charset=utf-8", end="\r\n\r\n")

page = """<!DOCTYPE html>
<html>
    <body>
    <h1>Calculatrice</h1>
    <form action="calcul.py" method="post">
        <input type="text" name="formule" value="" />
        <input type="submit" name="calc" value="Calculer">
    </form>
    </body>
</html>"""

print(page, end="")