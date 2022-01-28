#!/usr/bin/python3
import cgi

page = """<!DOCTYPE html>
<html>
    <body>
    <h1>Calculatrice</h1>
    <form action="calcul.py" method="post">
        <input type="text" name="formule" value="" />
        <input type="submit" name="calc" value="Calculer">
    </form>
    </body>
</html>"""s