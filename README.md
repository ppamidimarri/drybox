# Drybox
These scripts allow you to monitor the temperature and humidity in a set of dryboxes. This code allows you to monitor your dryboxes in a number of ways: 
* logs the readings to a database
* serve a website showing the readings in charts
* display the last readings on an LCD screen
* display color-coded LEDs based on the last readings

## Hardware Requirements
I use the following hardware for this project:
* One [Snapware Airtight 40-cup Rectangular Food Storage Container](https://smile.amazon.com/gp/product/B007V4IWIU) per drybox
* One [DHT22 sensor](https://smile.amazon.com/gp/product/B01N9BA0O4) per drybox 
* One [dehumidifier](https://smile.amazon.com/gp/product/B000H0XFCS) per drybox
* [Raspberry Pi 3B+](https://smile.amazon.com/gp/product/B07BDR5PDW)
* [20x4 I2C LCD module](https://smile.amazon.com/gp/product/B01GPUMP9C)
* [WS2812b LED strip](https://smile.amazon.com/gp/product/B01LSF4Q0A)
* [Arduino Nano](https://smile.amazon.com/gp/product/B0713XK923)
* [5V power supply](https://smile.amazon.com/gp/product/B078RT3ZPS) with enough current to power the LED strip
* 5V, 2A (or more) [Micro-USB power supply for the Pi](https://smile.amazon.com/gp/product/B075XMTQJC)
* USB to Mini-USB cable (to connect the Pi with the Arduino)
* Wires and Dupont connectors to connect everything up

## Software Used
* [Raspbian Stretch Lite](https://downloads.raspberrypi.org/raspbian_lite_latest)
* [nginx web server](https://www.nginx.com/resources/wiki/)
* [PHP](http://www.php.net/) for the website
* [MariaDB](https://mariadb.org/) for the database
* [Google Charts](https://developers.google.com/chart/) for interactive charts
* [FastLED.io](http://fastled.io/) library to control LEDs
* [Adafruit DHT](https://github.com/adafruit/DHT-sensor-library) library
* [I2C LCD Driver](https://github.com/emcniece/rpi-lcd/blob/master/RPi_I2C_driver.py) by Denis Pleic
* [Serial communication demo](http://forum.arduino.cc/index.php?topic=225329) by user robin2 at Arduino Forums

## 3D printed parts
[Thingiverse listing with STL files and printing instructions](https://www.thingiverse.com/thing:3456908)

## Instructions 
**Load a Pi with Raspbian Stretch Lite and get SSH going**

1. [Load Raspbian Stretch Lite on a Micro SD card](https://projects.raspberrypi.org/en/projects/raspberry-pi-setting-up)
2. [Set up image for SSH and WiFi](https://www.raspberrypi.org/forums/viewtopic.php?t=191252). 
3. Insert the Micro SD card into a Pi and boot it up. 
4. Establish SSH connection.
5. Change password, locale, timezone, etc. using `sudo raspi-config`
6. Enable I2C using `sudo raspi-config` and `sudo reboot`

**Update packages** 

1. `sudo apt update`
2. `sudo apt upgrade`

**Install new packages needed**

1. `sudo apt install python-dev python-pip mariadb-server python-mysqldb nginx php-fpm php-mbstring php-mysql raspberry-ui-mods smbus arduino`
2. `sudo pip install Adafruit_DHT pyserial`

**Set up the database**
1. Secure the database: `sudo mysql_secure_installation`
2. `sudo mysql -u root -p`
3. `create database drybox;`
4. `create user 'sensor'@'localhost' IDENTIFIED BY 'sensorpass'`;
5. `grant all privileges on drybox.* to 'sensor'@'localhost';`
6. Exit from mysql and now login as the user "sensor": `mysql -u sensor -p`
7. `use drybox;`
8. `CREATE TABLE readings (
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

**Download and set up the scripts**
1. Download the code from this repository using `wget` or `git clone`
2. Create folder drybox: `mkdir /home/pi/drybox`
3. Copy the python files into this new folder
4. Copy the `sketch_drybox_v4` folder into `/home/pi/drybox`
5. Copy the contents of the html folder to `/var/www/html`

**Connect everything up**
1. Setup the dryboxes with DHT22 sensors inside them.
2. Connect the DHT22 sensors to the Pi over GPIO pins. If you use different pins from what is shown in the picture below, edit `sensor.py` to update the pin numbers
5. Connect the LCD to the Pi using I2C pins.
3. Connect the Arduino Nano to the Pi using a USB-to-Mini-USB cable.
4. Connect the LED strip to the Arduino Nano over Pin 2. If you use a different pin, you need to update the sketch_drybox_v4.ino file with the number of the pin you are using

![Wiring Diagram](https://i.imgur.com/aFF4lY5.png)

**Load Arduino sketch to control LEDs**
1. Establish a GUI session with the Pi, either using Putty and Xming, or using an HDMI monitor plus USB keyboard and mouse
2. Start the Arduino IDE: `arduino`
3. Load the sketch called `sketch_drybox_v4.ino`
4. Compile and upload sketch to the Arduino Nano

**Test that things are working**
1. Run the sensor script with `python /home/pi/drybox/sensor.py`
2. The LCD should light up and within a minute or so, display the readings 
3. Login to the database with the `mysql` command above and check for recorded data: `select * from drybox.results;`
4. From your computer or phone browser, try the IP address of your Pi, it should show you the website like in the screenshots
5. The script will turn the LEDs above a drybox to red if the humidity reading from that DHT22 sensor is higher than a threshold. Based on your layout, you will need to specify which LEDs correspond to which drybox. You can do this in `sensor.py` by updating `HIGH_HUMIDITY`, `fil_starts` and `fil_ends`
5. Press Ctrl-C to kill the python script; the LCD should turn off

**[Install](https://www.raspberrypi.org/documentation/linux/usage/systemd.md) the service**
1. Copy the `dhtlogger.service` file: `sudo cp /home/pi/drybox/dhtlogger.service /etc/systemd/system/dhtlogger.service` 
2. Attempt to start the service: `sudo systemctl start dhtlogger.service`
3. Check that there are no errors, and check steps 2-4 from the "Test that things are working" section above
4. If everything is good, enable the service: `sudo systemctl enable dhtlogger.service`
5. Reboot the Pi with `sudo reboot` and check that it starts and check steps 2-4 from the "Test that things are working" section again

## [Pictures](https://imgur.com/a/YLTD9bh)
![Full setup](https://i.imgur.com/tljvpbC.jpg)
![Close-up of drybox](https://i.imgur.com/IOEDjfQ.jpg)
![Close-up of LCD](https://i.imgur.com/CsuUP5K.jpg)
![Website with charts](https://i.imgur.com/YcEIfC0.png)
