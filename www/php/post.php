
<head>
<link rel="shortcut icon" type="image/x-icon" href="/imgs/fire.ico"/>
	<title>Post</title>
	<link rel="stylesheet" href="/css/style.css">
</head>
<body>
	<header>
		<?php require('header.php'); ?>
	</header>
	<div class="avatar">
		<img src="/imgs/default.png">
	</div>
	<h2>Change your profile picture</h2>
	<form action="http://localhost:8080" method="post" enctype="multipart/form-data">
		<p><input type="file" id="myFile" name="filename" hidden>
		<label for="myFile">Upload</label>
		<span id="file-chosen">No file chosen</span>
		<p><button class="button" type="submit">Submit</button>
		<h2>Fill in your profile</h2>
		<p>First name <input type="text" name="firstname" /></p>
		<p>Last name <input type="text" name="lastname" /></p>
		<p>Login <input type="text" name="login" /></p>
		<p><input type="submit" value="OK"></p>
	</form>

</body>
<footer>
	<?php include('footer.php');
	?>
</footer>