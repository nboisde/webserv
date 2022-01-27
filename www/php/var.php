<head>
	<link rel="shortcut icon" type="image/x-icon" href="https://favicon-generator.org/favicon-generator/htdocs/favicons/2014-12-18/32e6e83989d480f4bed129837dec3f6c.ico" />
</head>

<?php
    while (list($var,$value) = each ($_SERVER)) {
        echo "$var => $value <br />";
    }
?>