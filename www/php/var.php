<header>
	<?php require('header.php'); ?>
</header>

<?php
    while (list($var,$value) = each ($_SERVER)) {
        echo "$var => $value <br />";
    }
?>