import pymysql
import matplotlib.pyplot as plt

class Database:

    def __init__(self,base_datos,host):
        self.connection =pymysql.connect(
            host = host,
            user = 'root',
            password = '',
            db = base_datos
        )
        self.cursor = self.connection.cursor()
        print("Conexion establecida")
        self.matriz = []
        self.lista =[]

    def conseguirDatos(self,id,idPaciente,tabla):
        try:
            sql = f'SELECT * FROM {tabla} WHERE id{tabla} = {id}'
            self.cursor.execute(sql)
            user = self.cursor.fetchone()
            for i in range(6):
                if idPaciente == user[1]:
                    self.lista.append(user[i])

            if idPaciente == user[1]:
                self.matriz.append(self.lista)
        except Exception as e:
            raise
        self.lista = []

    def conseguir_tamano(self, tabla):
        self.cursor.execute(f"SELECT COUNT(*) FROM {tabla}")
        valor = self.cursor.fetchone()
        return valor[0]


    def imprimir_datos_paciente(self,idPaciente,tabla):
        for i in range(self.conseguir_tamano(tabla)):
            self.conseguirDatos(i + 1, idPaciente, tabla)
        return self.matriz

database = Database('estacion_6','localhost')
database.imprimir_datos_paciente(2,"temperatura")
database2 = Database('estacion_6','localhost')
database3 = Database('estacion_6','localhost')
database2.imprimir_datos_paciente(1,"oximetria")
database3.imprimir_datos_paciente(1,"pulso")
print("-------------------------------------")


lista_tiempo=[]
lista_temperatura = []

def depuracion_de_datos_temperatura(matriz_datos):
    promedio =0
    tam = 0
    for i in range(len(database.matriz)):
        if str(database.matriz[i][3]) not in lista_tiempo:
            lista_tiempo.append(str(database.matriz[i][3]))
        try:
            if str(database.matriz[i][3])== str(database.matriz[i+1][3]):
                if "28" > str(database.matriz[i][3]) < "45":
                    promedio += database.matriz[i][5]
                    tam += 1
            if str(database.matriz[i][3])!= str(database.matriz[i+1][3]):
                if "28" > str(database.matriz[i][3]) < "45":
                    promedio += database.matriz[i][5]
                    tam += 1
                    lista_temperatura.append(promedio/tam)
        except Exception as e:
            lista_temperatura.append(promedio / tam)

lista_tiempo2=[]
lista_oximetria = []
def depuracion_de_datos_oximetria(matriz_objeto):

    promedio = 0
    tam = 0
    for i in range(len(database2.matriz)):
        if str(database2.matriz[i][3]) not in lista_tiempo2:
            lista_tiempo2.append(str(database2.matriz[i][3]))
        try:
            if str(database2.matriz[i][3]) == str(database2.matriz[i + 1][3]):
                if database2.matriz[i][5] > 75:
                    promedio += database2.matriz[i][5]
                    tam += 1
            if str(database2.matriz[i][3]) != str(database2.matriz[i + 1][3]):
                if database2.matriz[i][5] > 75:
                    promedio += database2.matriz[i][5]
                    tam += 1
                lista_oximetria.append(promedio / tam)
        except Exception as e:
            lista_oximetria.append(promedio / tam)

lista_tiempo3=[]
lista_pulso = []
def depuracion_de_datos_pulso(matriz_objeto):
    promedio = 0
    tam = 0
    for i in range(len(database2.matriz)):
        if str(database2.matriz[i][3]) not in lista_tiempo3:
            lista_tiempo3.append(str(database2.matriz[i][3]))
        try:
            if str(database2.matriz[i][3]) == str(database2.matriz[i + 1][3]):
                if database2.matriz[i][5] > 40:
                    promedio += database2.matriz[i][5]
                    tam += 1
            if str(database2.matriz[i][3]) != str(database2.matriz[i + 1][3]):
                if database2.matriz[i][5] > 40:
                    promedio += database2.matriz[i][5]
                    tam += 1
                lista_pulso.append(promedio / tam)
        except Exception as e:
            lista_pulso.append(promedio / tam)



depuracion_de_datos_temperatura("1")
plt.plot(lista_tiempo,lista_temperatura,"red",label="Temperatura")
plt.legend(['Temperatura °C', 'Second List'], loc='upper left')
plt.ylim(30,40)
plt.show()

depuracion_de_datos_oximetria("1")
oximetria = plt.plot(lista_tiempo2,lista_oximetria,"blue",label="Oximetria")
plt.legend(['Oximetria %', 'Second List'], loc='upper left')
plt.ylim(50,100)
plt.show()

depuracion_de_datos_pulso('1')
pulso = plt.plot(lista_tiempo3, lista_pulso,"green",label="Pulso")
plt.legend(['Pulso HR', 'Second List'], loc='upper left')
plt.ylim(50, 100)
plt.show()


