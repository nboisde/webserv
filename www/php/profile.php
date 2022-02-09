<head>
<link rel="shortcut icon" type="image/x-icon" href="/imgs/favicon.ico"/>
	<title>Profile</title>
	<link rel="stylesheet" href="/css/style.css">
</head>
<body>
<header>
	<?php require('header.php'); ?>
</header>
	<h1>Welcome to your profile</h1>

	<div class="avatar">
		<img src="/imgs/default.png">
	</div>
	<form action="action.php" method="post">
		<p>FIRST NAME <?php echo htmlspecialchars($_POST['firstname']);?></p>
		<p>LAST NAME <?php echo htmlspecialchars($_POST['lastname']); ?></p>
		<p>LOGIN <?php echo htmlspecialchars($_POST['login']);?></p>
	</form>
	<div>
		<a href="/php/post.php" class="button">EDIT</a>
	</div>
</body>
<footer>
	<?php include('footer.php');
	?>
</footer>