import serial, time

l1 = b"1.1.0.0.0.100.1.0.255.255.1."

port = "COM" + input("ESP32 port: COM")

data_serial = serial.Serial(port, 115600)

while True:
    esp32_data = data_serial.readline().decode("ascii")
    print(esp32_data)
    data_serial.write(l1)