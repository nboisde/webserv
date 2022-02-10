
<head>
<link rel="shortcut icon" type="image/x-icon" href="/imgs/fire.ico"/>
	<title>Homepage</title>
	<link rel="stylesheet" href="/css/style.css">
</head>
<body>
	<header>
		<?php include('header.php'); ?>
	</header>
	<h1>Welcome to our<br> Webserver</h1>
	<div class="content">
		<p>Le protocole HTTP (Hypertext Transfer Protocol) est un protocole d’application pour
les systèmes d’information distribués, collaboratifs et hypermédia. HTTP est la base de
la communication de données pour le World Wide Web, où les documents hypertextes
incluent des hyperliens vers d’autres ressources auxquelles l’utilisateur peut facilement accéder, par exemple par un clic de souris ou en tapant sur l’écran dans un navigateur Web.
HTTP a été développé pour faciliter l’hypertexte et le World Wide Web.<br><br>
La fonction principale d’un serveur Web est de stocker, traiter et livrer des pages Web
aux clients.<br><br>
La communication entre le client et le serveur s’effectue à l’aide du protocole HTTP
(Hypertext Transfer Protocol).<br><br>
Les pages livrées sont le plus souvent des documents HTML, qui peuvent inclure des
images, des feuilles de style et des scripts en plus du contenu textuel.
Plusieurs serveurs Web peuvent être utilisés pour un site Web à fort trafic.
Un agent d’utilisateur, généralement un navigateur Web ou un robot d’indexation
Web, initie la communication en faisant une demande pour une ressource spécifique à
l’aide de HTTP.<br><br>
Le serveur répond par le contenu de cette ressource ou par un message d’erreur s’il
est incapable de le faire. La ressource est généralement un fichier réel sur le stockage
secondaire du serveur, mais ce n’est pas nécessairement le cas et dépend de la manière
dont le serveur Web est implémenté.</p>
	</div>
	<div class="methods">
		<div>
			<a href="/php/get.php" class="button">GET</a>
		</div>
		<div>
			<a href="/php/post.php" class="button">POST</a>
		</div>
		<div>
			<a href="/php/delete.php" class="button">DELETE</a>
		</div>
	</div>
</body>
<footer>
	<?php include('footer.php');
	?>
</footer>

