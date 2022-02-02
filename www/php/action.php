<head>
        <link rel="shortcut icon" type="image/x-icon" href="/imgs/fire.ico" />
        <title>Action</title>
	    <link rel="stylesheet" href="/css/style.css">
</head>
Bonjour et bienvenue, <?php echo htmlspecialchars($_POST['nom']); ?>.
Tu as <?php echo (int)$_POST['age']; ?> ans.