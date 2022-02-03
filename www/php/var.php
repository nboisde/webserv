<head>
        <link rel="shortcut icon" type="image/x-icon" href="/imgs/fire.ico" />
        <title>Var</title>
	    <link rel="stylesheet" href="/css/style.css">
        <ul>
		    <li><a href="/html/contact.html">Contact</a></li>
		    <li><a href="/html/about.html">About</a></li>
		    <li><a href="/html/index.html">Home</a></li>
	    </ul>
</head>

<?php
    while (list($var,$value) = each ($_SERVER)) {
        echo "$var => $value <br />";
    }
?>