import serial
import time
import urllib.request

DataSerial = serial.Serial('COM3' , 9600)

time.sleep(1)

# ThingSpeak parameters
base_url = "https://api.thingspeak.com/update"
api_key = "3BFCB4I0KRU8DH57"

while True:
    while DataSerial.inWaiting() == 0:
        pass

    data = DataSerial.readline()
    data = str(data, 'utf-8')
    data = data.strip('\r\n')

    print(data)

    # Prepare the URL to send data to ThingSpeak
    url = base_url + "?api_key=" + api_key
    url += "&field2=" + data

    # Send the data to ThingSpeak
    urllib.request.urlopen(url)

    time.sleep(1)  # Delay between sending data (adjust as needed)