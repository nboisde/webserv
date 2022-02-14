
<head>
<link rel="shortcut icon" type="image/x-icon" href="/imgs/fire.ico"/>
	<title>Post</title>
	<link rel="stylesheet" href="/css/style.css">
</head>
<body>
	<header>
		<?php require('header.php'); ?>
	</header>
	<h1>Edit your profile</h1>
	<div class="avatar">
		<img src="/imgs/default.png">
	</div>
	<form action="/php/profile.php" method="post" enctype="multipart/form-data">
		<p><input type="file" id="myFile" name="filename" hidden>
		<label for="myFile">Upload</label>
		<p></p>
		<span id="file-chosen">No file chosen</span>
		<p>First name <input type="text" name="firstname" /></p>
		<p>Last name <input type="text" name="lastname" /></p>
		<p>Login <input type="text" name="login" /></p>
		<p><input type="submit" id="validation" value="OK" hidden></p>
		<label for="validation">SUBMIT</label>
	</form>

</body>
<footer>
	<?php include('footer.php');
	?>
</footer>