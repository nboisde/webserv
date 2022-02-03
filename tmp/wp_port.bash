#!/bin/bash

address="http://127.0.0.1"

sed -i "/define('WPSITEURL',/d" ../www/wordpress/wp-config.php
sed -i "/define('WPHOME',/d" ../www/wordpress/wp-config.php

echo "define('WPSITEURL','$address:$1/');" >> ../www/wordpress/wp-config.php
echo "define('WPHOME','$address:$1/');" >> ../www/wordpress/wp-config.php

echo "$address:$1"

echo "CREATE DATABASE IF NOT EXISTS wordpress;
use wordpress;
update wp_options set option_value='$address:$1' where option_name='siteurl';
update wp_options set option_value='$address:$1' where option_name='home';" >> tmp_script.sql

sudo mysql -u root -puser42 < tmp_script.sql

rm -rf tmp_script.sql
