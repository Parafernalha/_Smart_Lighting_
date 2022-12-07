import requests, json, serial, serial.tools.list_ports, time

print("__"*30)

ports = serial.tools.list_ports.comports()
serialInst = serial.Serial()

portList = []

for onePort in ports:
    portList.append(str(onePort))
    print(str(onePort))

print("__"*30)

while True:
    try:
        val = "COM" + input("Selecione a porta correspondente: COM")
        data_serial = serial.Serial(val, 115600)
        break
    except:
        print("__"*30)
        print("Porta inexistente. Tente novamente!")

link = "https://jornadasextoperiodo-default-rtdb.firebaseio.com/"

lista = []
dic = {}
x = 0

print("__"*30)
print("Iniciando...")

while True:
    try:
        esp32_data = data_serial.readline().decode("ascii")
        lista = esp32_data.split(".")
        lista.pop()
        dic = {"temp": lista[2], "humi": lista[3], "lumens": lista[4], "motion": lista[5]}
        requisição = requests.patch(f"{link}/L1/.json", data=json.dumps(dic))

        requisição = requests.get(f"{link}/L1/.json")
        requisição = requisição.json()
        getMontado = "1"+"."+"1"+"."+str(requisição["auto"])+"."+str(requisição["adap"])+"."+str(requisição["movi"])+"."+str(requisição["number"])+"."+str(requisição["ajuste"])+"."+str(requisição["ajustemin"])+"."+str(requisição["ajustemax"])+"."+str(requisição["ajusteLumens"])+"."+str(requisição["tempoMovimento"])+"."
        data_serial.write(getMontado.encode('utf-8'))
        if x == 0:
            print("Conexão bem sucedida!")
            print("__"*30)
            print("Em processo...")
        x = 1
    except:
        if x == 1:
            break
        x = 0


