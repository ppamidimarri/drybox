# drybox
These scripts allow you to monitor the temperature and humidity in a set of dryboxes. This code allows you to monitor your dryboxes in a number of ways: 
* logs the readings to a database
* serve a website showing the readings in charts
* display the last readings on an LCD screen
* display color-coded LEDs based on the last readings

## Hardware Requirements
I use the following hardware for this project:
* Raspberry Pi 3B+
* Arduino Nano
* DHT22 sensors
* 20x4 I2C LCD module
* WS2812B LED strip with its own 5V power supply
* 

## Software Used
* Raspbian Stretch Lite
* nginx web server
* PHP for the website
* MariaDB for the database
* Google Charts for interactive charts
* FastLED.io library to control LEDs
* Adafruit DHT library
* I2C LCD Driver by Denis Pleic
* [Serial communication demo](http://forum.arduino.cc/index.php?topic=225329) by user robin2 at Arduino Forums

## 3D printed parts
https://www.thingiverse.com/thing:3456908

## Instructions 
**Load a Pi with Raspbian Stretch Lite and get SSH going**

1. [Load Raspbian Stretch Lite on a Micro SD card](https://projects.raspberrypi.org/en/projects/raspberry-pi-setting-up)
2. [Set up image for SSH and WiFi](https://www.raspberrypi.org/forums/viewtopic.php?t=191252). 
3. Insert the Micro SD card into a Pi and boot it up. 
4. Establish SSH connection.

**Update packages** 

1. `sudo apt update`
2. `sudo apt upgrade`

**Install new packages needed**

1. `sudo apt install python-dev python-pip mariadb-server python-mysqldb nginx php-fpm php-mbstring php-mysql raspberry-ui-mods arduino`
2. `sudo pip install Adafruit_DHT pyserial`

**Set up the database**
1. Secure the database: `sudo mysql_secure_installation`
2. `mysql -u root -p`
3. `create database drybox;`
4. `create user 'sensor'@'localhost' IDENTIFIED BY 'sensorpass'`;
5. `grant all privileges on drybox.* to 'sensor'@'localhost';`
6. `mysql -u sensor -p`
7. `CREATE TABLE readings (
  id int(11) NOT NULL AUTO_INCREMENT,
  stamp timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  humidity_1 float DEFAULT NULL,
  temperature_1 float DEFAULT NULL,
  humidity_2 float DEFAULT NULL,
  temperature_2 float DEFAULT NULL,
  humidity_3 float DEFAULT NULL,
  temperature_3 float DEFAULT NULL,
  humidity_4 float DEFAULT NULL,
  temperature_4 float DEFAULT NULL,
  humidity_5 float DEFAULT NULL,
  temperature_5 float DEFAULT NULL,
  PRIMARY KEY (id)
);`

**Configure php-fpm**
`sudo nano /etc/php/7.0/fpm/php.ini`, change `#cgi.fix_pathinfo=1` to `cgi.fix_pathinfo=0`

**Configure nginx**
`sudo nano /etc/nginx/sites-available/default`, edit these lines to look like:

      index index.html index.htm index.php index.nginx-debian.html;
      location ~ \.php$ {
         include snippets/fastcgi-php.conf;
         # With php-fpm (or other unix sockets):
         fastcgi_pass unix:/var/run/php/php7.0-fpm.sock;
      }

## Pictures
[Screenshots](https://imgur.com/a/YLTD9bh)
