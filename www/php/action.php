<head>
        <link rel="shortcut icon" type="image/x-icon" href="https://favicon-generator.org/favicon-generator/htdocs/favicons/2014-12-18/32e6e83989d480f4bed129837dec3f6c.ico" />
        <title>Welcome</title>
	    <link rel="stylesheet" href="style.css">
</head>

Bonjour et bienvenue, <?php echo htmlspecialchars($_POST['nom']); ?>.
Tu as <?php echo (int)$_POST['age']; ?> ans.
