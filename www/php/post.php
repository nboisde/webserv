
<head>
<link rel="shortcut icon" type="image/x-icon" href="/imgs/fire.ico"/>
	<title>Post</title>
	<link rel="stylesheet" href="/css/style.css">
</head>
<body>
	<header>
		<?php require('header.php'); ?>
	</header>
	<img src="/imgs/cute.png">
	<h2>Fill in your profile</h2>
	<form action="/php/profile.php" method="post">	
		<p>First name <input type="text" name="firstname" /></p>
		<p>Last name <input type="text" name="lastname" /></p>
		<p>Login <input type="text" name="login" /></p>
		<p><input type="submit" value="OK"></p>
	</form>
	<h2>Upload your picture</h2>
	<form action="http://localhost:8080" method="post" enctype="multipart/form-data">
		<p><input type="file" name="file1">
		<p><button class="styled" type="submit">Submit</button>
	</form>
</body>
<footer>
	<?php include('footer.php');
	?>
</footer>