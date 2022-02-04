#!/bin/bash

wget https://fr.wordpress.org/latest-fr_FR.tar.gz -P ../www/
tar -xzf ../www/latest-fr_FR.tar.gz -C ../www/
rm ../www/latest-fr_FR.tar.gz
cp wp-config.php ../www/wordpress/wp-config.php
