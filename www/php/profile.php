<head>
	<link rel="stylesheet" href="/css/style.css">
</head>
<body>
<header>
	<?php require('header.php'); ?>
</header>
<img src="/imgs/cute.png">
	<form action="action.php" method="post">
		<p>Hello and welcome <?php echo htmlspecialchars($_POST['firstname']); ?> <?php echo htmlspecialchars($_POST['lastname']); ?></p>
		<p>(aka <?php echo htmlspecialchars($_POST['login']); ?>)</p>
		<p><input type="submit" value="OK"></p>
	</form>
</body>
<footer>
	<?php include('footer.php');
	?>
</footer>