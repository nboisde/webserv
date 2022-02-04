<head>
	<link rel="shortcut icon" type="image/x-icon" href="/imgs/fire.ico"/>
	<title>Contact</title>
	<link rel="stylesheet" href="/css/style.css">
	<ul>
		<li><a href="/html/contact.html">Contact</a></li>
		<li><a href="/html/about.html">About</a></li>
		<li><a style="text-decoration: underline;" href="/php/profile.php">Profile</a></li>
		<li><a href="/html/index.html">Home</a></li>
	</ul>
</head>
<body>
	<img src="/imgs/cute.png">
	<form action="action.php" method="post">
		<p>Hello and welcome <?php echo htmlspecialchars($_POST['firstname']); ?> <?php echo htmlspecialchars($_POST['lastname']); ?></p>
		<p>(aka <?php echo htmlspecialchars($_POST['login']); ?>)</p>
		<p><input type="submit" value="OK"></p>
	</form>
</body>