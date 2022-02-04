<head>
        <link rel="shortcut icon" type="image/x-icon" href="/imgs/fire.ico" />
        <title>Action</title>
	<link rel="stylesheet" href="/css/style.css">
        <ul>
		<li><a href="/html/contact.html">Contact</a></li>
		<li><a href="/html/about.html">About</a></li>
		<li><a href="/html/index.html">Home</a></li>
	</ul>
</head>
<body>
	<img src="/imgs/cute.png">
	<form action="action.php" method="post">
		Hello and welcome <?php echo htmlspecialchars($_POST['nom']); ?> <?php echo htmlspecialchars($_POST['lastname']); ?>
		AKA <?php echo htmlspecialchars($_POST['login']); ?>
		<p><input type="submit" value="OK"></p>
	</form>
</body>