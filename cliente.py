from pyModbusTCP.client import ModbusClient
from pyModbusTCP import utils
import time
import serial

class FloatModbusClient(ModbusClient):
    def read_float(self, address, number=1):
        reg_l = self.read_holding_registers(address, number * 2)
        if reg_l:
            return [utils.decode_ieee(f) for f in utils.word_list_to_long(reg_l)]
        else:
            return None

    def write_float(self, address, floats_list):
        b32_l = [utils.encode_ieee(f) for f in floats_list]
        b16_l = utils.long_list_to_word(b32_l)
        return self.write_multiple_registers(address, b16_l)


c = FloatModbusClient(host='localhost', port=502, auto_open=True)

SERVER_HOST = "localhost"
SERVER_PORT = 502

c.host(SERVER_HOST)
c.port(SERVER_PORT)

ser = serial.Serial('COM3', 9600)

while (1):            
    leitura_serial = ser.readline()
    try:
        valores = leitura_serial.split('/')
        print(valores)
        nivel = float(valores[1])
        valorVin = float(valores[2])
        valorVout = float(valores[3])
        
        if not c.is_open():
            if not c.open():
                print("unable to connect to "+SERVER_HOST+":"+str(SERVER_PORT))
        if c.is_open():
            c.write_float(0, [nivel])
            c.write_float(1, [valorVin])
            c.write_float(2, [valorVout])    
    except:
        pass
    
        
        