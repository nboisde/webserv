<?php
/**
 * La configuration de base de votre installation WordPress.
 *
 * Ce fichier est utilisé par le script de création de wp-config.php pendant
 * le processus d’installation. Vous n’avez pas à utiliser le site web, vous
 * pouvez simplement renommer ce fichier en « wp-config.php » et remplir les
 * valeurs.
 *
 * Ce fichier contient les réglages de configuration suivants :
 *
 * Réglages MySQL
 * Préfixe de table
 * Clés secrètes
 * Langue utilisée
 * ABSPATH
 *
 * @link https://fr.wordpress.org/support/article/editing-wp-config-php/.
 *
 * @package WordPress
 */

// ** Réglages MySQL - Votre hébergeur doit vous fournir ces informations. ** //
/** Nom de la base de données de WordPress. */
define( 'DB_NAME', 'wordpress' );

/** Utilisateur de la base de données MySQL. */
define( 'DB_USER', 'user' );

/** Mot de passe de la base de données MySQL. */
define( 'DB_PASSWORD', 'password' );

/** Adresse de l’hébergement MySQL. */
define( 'DB_HOST', 'localhost:3306' );

/** Jeu de caractères à utiliser par la base de données lors de la création des tables. */
define( 'DB_CHARSET', 'utf8' );

/**
 * Type de collation de la base de données.
 * N’y touchez que si vous savez ce que vous faites.
 */
define( 'DB_COLLATE', '' );

/**#@+
 * Clés uniques d’authentification et salage.
 *
 * Remplacez les valeurs par défaut par des phrases uniques !
 * Vous pouvez générer des phrases aléatoires en utilisant
 * {@link https://api.wordpress.org/secret-key/1.1/salt/ le service de clés secrètes de WordPress.org}.
 * Vous pouvez modifier ces phrases à n’importe quel moment, afin d’invalider tous les cookies existants.
 * Cela forcera également tous les utilisateurs à se reconnecter.
 *
 * @since 2.6.0
 */
define('AUTH_KEY',         'itP*oa5H *b]YAqfPu|k[*QT-gW6X, 6T66IpOwtfv{n~40Ud`_N4]?p`_R&&46U');
  define('SECURE_AUTH_KEY',  'g(Egw%ztzJo{t0MxMlm_AtVO:Ln[3^vjTKA;23@F<<W>BdB6w]+qWT+Lwy!Qd$w{');
  define('LOGGED_IN_KEY',    '8i<ez&G8?ch23t?DW)nv3S,[4=I7~)OTeO.5e%Bld]{b9|ZY{Jl=`ANmorN_O]Ta');
  define('NONCE_KEY',        '/I^[n/ (KkAPfMOn+!NC^U7Z+b{6zy>[iDO?FY.KAW[T!9;?BTr]$gS,b7JrucoO');
  define('AUTH_SALT',        'cV+jD&DGU!aubf%=,v-NUGOd] ?0)_Fj4qMqZLy>fAX`lJlz;.:cM7z$|ue= &71');
  define('SECURE_AUTH_SALT', '7[ECVF^W-g?O>NA!f&@c0.UyXPB|HhBw2W+u63u,F#y-+*K`-J7e-P3{|}.[49(r');
  define('LOGGED_IN_SALT',   '5c>X3>:@_]4V5$p:!jpFd!mwAwoG5+m7WHs,M9<G<3Ms1/wh?a@EP9u[Y +..-/A');
  define('NONCE_SALT',       'J#9I+Z g$eVoJy1lvoyq~c.9&ridm@42LQ.WB`uEB!3X`$-+T1V2[>=)in,xhs`&');
/**#@-*/

/**
 * Préfixe de base de données pour les tables de WordPress.
 *
 * Vous pouvez installer plusieurs WordPress sur une seule base de données
 * si vous leur donnez chacune un préfixe unique.
 * N’utilisez que des chiffres, des lettres non-accentuées, et des caractères soulignés !
 */
$table_prefix = 'wp_';

/**
 * Pour les développeurs : le mode déboguage de WordPress.
 *
 * En passant la valeur suivante à "true", vous activez l’affichage des
 * notifications d’erreurs pendant vos essais.
 * Il est fortement recommandé que les développeurs d’extensions et
 * de thèmes se servent de WP_DEBUG dans leur environnement de
 * développement.
 *
 * Pour plus d’information sur les autres constantes qui peuvent être utilisées
 * pour le déboguage, rendez-vous sur le Codex.
 *
 * @link https://fr.wordpress.org/support/article/debugging-in-wordpress/
 */
define( 'WP_DEBUG', false );

/* C’est tout, ne touchez pas à ce qui suit ! Bonne publication. */

/** Chemin absolu vers le dossier de WordPress. */
if ( ! defined( 'ABSPATH' ) )
  define( 'ABSPATH', dirname( __FILE__ ) . '/' );

/** Réglage des variables de WordPress et de ses fichiers inclus. */
require_once( ABSPATH . 'wp-settings.php' );
