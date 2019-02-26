#!/usr/bin/env python
#--------------------------------------
# Show on a 20x4 LCD humidity and
# temperature readings obtained from a
# set of DHT22 sensors
#--------------------------------------

# Imports
import i2c_lcd_driver
import serial_driver
import time
import Adafruit_DHT as dht
from datetime import datetime
import MySQLdb as mysql
import signal

# DHT22 sensor constants
DHT22_PINS = [17, 14, 15, 4, 18]
HIGH_HUMIDITY = 20

# Database connection and cursor for logging: database is "drybox" and table is "readings"
db = mysql.connect("localhost", "sensor", "sensorpass", "drybox")
db_cursor = db.cursor()

# LCD settings
mylcd = i2c_lcd_driver.lcd()

# Serial settings to talk to Arduino controlling the LEDs
fil_starts = [20, 34, 48, 62, 75]
fil_ends = [25, 39, 53, 67, 80]
ser = serial_driver.serial_device("/dev/ttyUSB0", 57600)
baseColor = chr(32) + chr(32) + chr(32)
highHumidityColor = chr(1) + chr(255) + chr(1)

def exit_gracefully(signum, frame):
	db.close()
	mylcd.lcd_clear()
	mylcd.backlight(0)
	ser.close()
	exit()

def main():
	# Process exit gracefully
	signal.signal(signal.SIGINT, exit_gracefully)
	signal.signal(signal.SIGTERM, exit_gracefully)

	# Clear the LCD and turn the backlight on
	mylcd.lcd_clear()
	mylcd.backlight(1)

	while True:
		# Read humidity and temperature from each DHT22
		humi = []
		temp = []
		for pin in DHT22_PINS:
			h, t = dht.read_retry(dht.DHT22, pin)
			humi.append(h)
			temp.append(t)
		head_str = "   1   2   3   4   5"
		# Send text to LCD
		mylcd.lcd_display_string(head_str, 1)
		mylcd.lcd_display_string(format_humidity_line(humi), 2)
		mylcd.lcd_display_string(format_temperature_line(temp), 3)
		mylcd.lcd_display_string(format_status_line(humi, temp), 4)
		# Wait for 10 minutes
		time.sleep(600)

def log_high_humidity(humi, temp):
	now = datetime.now()
	today = now.strftime("%Y-%m-%d")
	timeofday = now.strftime("%H:%M:%S")
	for i in range(5):
		h = None
		t = None
		if humi[i] is None or humi[i] > 100 or humi[i] < 0:
			h = "NULL"
		else:
			h = humi[i]
		if temp[i] is None:
			t = "NULL"
		else:
			t = fahrenheit(temp[i])
		sql_command = "insert into readings (stamp_date, stamp_time, sensor, temperature, humidity) values("
		sql_command += "'{0}', '{1}', {2:0d}, {3}, {4})".format(today, timeofday, i+1, t, h)
		try:
			db_cursor.execute(sql_command)
			db.commit();
		except:
			print("ERROR executing SQL: " + sql_command)
			db.rollback()
	return True

def make_default_color():
	return baseColor

def make_filament_color(humidity):
	if humidity >= HIGH_HUMIDITY:
		return highHumidityColor
	else:
		return baseColor

def make_serial_message(humi):
	data = ""
	data += make_default_color()
	for i in range(5):
		data += chr(fil_starts[i])
		data += chr(fil_ends[i])
		data += make_filament_color(humi[i])
	return data

def send_light_colors(humi, temp):
	message = make_serial_message(humi);
	print("Sending to Arduino: " + message)
	ser.sendToArduino(message)
	time.sleep(1)
	if ser.inWaiting() > 0:
		dataRecvd = ser.recvFromArduino()

		if dataRecvd[0] == 0:
			ser.displayDebug(dataRecvd[1])

		if dataRecvd[0] > 0:
			ser.displayData(dataRecvd[1])
			print("Reply Received")

def format_status_line(humi, temp):
	log_high_humidity(humi, temp)
	send_light_colors(humi, temp)
	now = datetime.now()
	return now.strftime("%b %-d, %-I:%M %p").center(20)

def format_humidity_line(humi):
	line = "%"
	for reading in humi:
		if reading is None:
			line += "    "
		else:
			line += "{0:>3.0f} ".format(reading)
	return line[:-1] # remove final space for formatting

def format_temperature_line(temp):
	line = "F"
	for reading in temp:
		if reading is None:
			line += "    "
		else:
			line += "{0:>3.0f} ".format(fahrenheit(reading))
	return line[:-1] # remove final space for formatting

def fahrenheit(celsius):
	return celsius*9/5+32

if __name__ == '__main__':
	try:
		main()
	except KeyboardInterrupt:
		exit_gracefully()
