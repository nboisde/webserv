<?php
		if (isset($_POST['firstname']))
		{
			$cookie_name1 = "firstname";
			$cookie_value1 = $_POST['firstname'];
			setcookie($cookie_name1, $cookie_value1, time() + (86400 * 30), "/"); // 86400 = 1 day
		}
		if (isset($_POST['lastname']))
		{
			$cookie_name2 = "lastname";
			$cookie_value2 = $_POST['lastname'];
			setcookie($cookie_name2, $cookie_value2, time() + (86400 * 30), "/"); // 86400 = 1 day
		}
		if (isset($_POST['login']))
		{
			$cookie_name3 = "login";
			$cookie_value3 = $_POST['login'];
			setcookie($cookie_name3, $cookie_value3, time() + (86400 * 30), "/"); // 86400 = 1 day
		}
?>
<html>
<head>
<link rel="shortcut icon" type="image/x-icon" href="/imgs/logo.png"/>
	<title>Profile</title>
	<link rel="stylesheet" href="/css/style.css">
</head>
<body>
<header>
	<?php require('header.php'); ?>
</header>
	<h1>Welcome to your profile</h1>

	<label class="profil">
		<img src="/imgs/cute.png">
	</label>

	<form action="action.php" method="post">
		<p>FIRST NAME <?php
			if (isset($_POST["firstname"])){
				echo htmlspecialchars($_POST["firstname"]);
			}
			else if (isset($_COOKIE["firstname"])){
				echo htmlspecialchars($_COOKIE["firstname"]);
			}	
			else {
				echo "John";
			}
			?></p>
		<p>LAST NAME <?php 
			if (isset($_POST["lastname"]))
			{
				echo htmlspecialchars($_POST["lastname"]);
			}
			else if (isset($_COOKIE["lastname"]))
			{	
				echo htmlspecialchars($_COOKIE["lastname"]);
			}
			else
			{
				echo "Doe";
			}
			?></p>
		<p>LOGIN <?php
			if (isset($_POST["login"]))
			{
				echo htmlspecialchars($_POST["login"]);
			}
			else if (isset($_COOKIE["login"]))
			{
				echo htmlspecialchars($_COOKIE["login"]);
			}
			else
			{
				echo "jdoe";
			}
			?></p>
	</form>
	<div>
		<a href="/php/post.php" class="button">EDIT</a>
	</div>
<footer>
	<?php include('footer.php');
	?>
</footer>
</body>
</html>