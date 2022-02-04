#!/bin/bash

wget https://fr.wordpress.org/latest-fr_FR.tar.gz -P ../www/
tar -xzf ../www/latest-fr_FR.tar.gz -C ../www/
rm ../www/latest-fr_FR.tar.gz
cp wp-config.php ../www/wordpress/wp-config.php

sudo mysql -u root <<EOF
create database wordpress default character set utf8 collate utf8_unicode_ci;
create user 'user'@'localhost';
GRANT ALL PRIVILEGES ON wordpress.* to 'user'@'localhost';
FLUSH PRIVILEGES;
EOF

