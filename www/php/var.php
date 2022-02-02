<head>
        <link rel="shortcut icon" type="image/x-icon" href="/imgs/fire.ico" />
        <title>Var</title>
	    <link rel="stylesheet" href="/css/style.css">
    </head>

<?php
    while (list($var,$value) = each ($_SERVER)) {
        echo "$var => $value <br />";
    }
?>