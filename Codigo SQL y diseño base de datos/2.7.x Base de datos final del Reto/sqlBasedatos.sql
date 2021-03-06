CREATE DATABASE IF NOT EXISTS estacion_6;
USE estacion_6;
CREATE TABLE IF NOT EXISTS especialista(
    idEspecialista INT NOT NULL AUTO_INCREMENT,
    nombre VARCHAR(40) NOT NULL,
    apellido VARCHAR(40) NOT NULL,
    especialidad VARCHAR(40) NOT NULL,
    PRIMARY KEY (idEspecialista)
);
CREATE TABLE IF NOT EXISTS paciente(
    idPaciente INT NOT NULL AUTO_INCREMENT,
    idEspecialista INT NOT NULL,
    nombre VARCHAR(40) NOT NULL,
    apellido VARCHAR(40) NOT NULL,
    fecha_nacimiento DATE NOT NULL,
    PRIMARY KEY (idPaciente),
    FOREIGN KEY (idEspecialista)
    REFERENCES especialista (idEspecialista)
);

CREATE TABLE IF NOT EXISTS estacion_monitoreo (
    idEstacion INT NOT NULL AUTO_INCREMENT,
    modelo VARCHAR(40) NOT NULL,
    PRIMARY KEY (idEstacion)
);

CREATE TABLE IF NOT EXISTS sensor_pulso_oximetria (
    idsensorpulso_oxi INT NOT NULL AUTO_INCREMENT,
    idEstacion INT NOT NULL,
    modelo VARCHAR(40) NOT NULL,
    PRIMARY KEY (idsensorpulso_oxi),
    FOREIGN KEY (idEstacion)
    REFERENCES estacion_monitoreo (idEstacion)
    
);

CREATE TABLE IF NOT EXISTS sensor_temperatura (
    idsensorTemperatura INT NOT NULL AUTO_INCREMENT,
    idEstacion INT NOT NULL,
    modelo VARCHAR(40) NOT NULL,
    PRIMARY KEY (idsensorTemperatura),
    FOREIGN KEY (idEstacion)
    REFERENCES estacion_monitoreo (idEstacion)
);


CREATE TABLE IF NOT EXISTS temperatura (
	idTemperatura INT NOT NULL AUTO_INCREMENT,
    idPaciente INT NOT NULL,
    idsensorTemperatura INT NOT NULL,
    fecha DATE NOT NULL DEFAULT CURRENT_DATE,
    hora TIME NOT NULL DEFAULT CURRENT_TIME,
    valor INT NOT NULL,
    PRIMARY KEY (idTemperatura),
    FOREIGN KEY (idPaciente)
    REFERENCES paciente (idPaciente),
    FOREIGN KEY (idsensorTemperatura)
    REFERENCES sensor_temperatura (idsensorTemperatura)
);


CREATE TABLE IF NOT EXISTS pulso (
    idPulso INT NOT NULL AUTO_INCREMENT,
    idPaciente INT NOT NULL,
    idsensorpulso_oxi INT NOT NULL,
    fecha DATE NOT NULL DEFAULT CURRENT_DATE,
    hora TIME NOT NULL DEFAULT CURRENT_TIME,
    valor INT NOT NULL,
    PRIMARY KEY (idPulso),
    FOREIGN KEY (idPaciente)
    REFERENCES paciente (idPaciente),
    FOREIGN KEY (idsensorpulso_oxi)
    REFERENCES sensor_pulso_oximetria (idsensorpulso_oxi)
);

CREATE TABLE IF NOT EXISTS oximetria (
    idOximetria INT NOT NULL AUTO_INCREMENT,
    idPaciente INT NOT NULL,
    idsensorpulso_oxi INT NOT NULL,
    fecha DATE NOT NULL DEFAULT CURRENT_DATE,
    hora TIME NOT NULL DEFAULT CURRENT_TIME,
    valor FLOAT NOT NULL,
    PRIMARY KEY (idOximetria),
    FOREIGN KEY (idPaciente)
    REFERENCES paciente (idPaciente),
    FOREIGN KEY (idsensorpulso_oxi)
    REFERENCES sensor_pulso_oximetria (idsensorpulso_oxi)
);

INSERT INTO estacion_monitoreo  (modelo

) VALUES ('NodeMCU'

);

INSERT INTO sensor_pulso_oximetria  
   SET idsensorpulso_oxi  = 1,
       idEstacion  = (
       SELECT idEstacion 
         FROM estacion_monitoreo 
        WHERE idEstacion  = LAST_INSERT_ID()),
	modelo = 'MAX3020';

INSERT INTO sensor_temperatura   
   SET idsensorTemperatura   = 1,
       idEstacion  = (
       SELECT idEstacion 
         FROM estacion_monitoreo 
        WHERE idEstacion  = LAST_INSERT_ID()),
	modelo = 'DHT11';


INSERT INTO especialista (nombre,apellido,especialidad

) VALUES ('Carlitos','Buendia','Internista'

);
INSERT INTO paciente 
   SET idpaciente = 1,
       idespecialista = (
       SELECT idespecialista
         FROM especialista
        WHERE idespecialista = LAST_INSERT_ID()),
	nombre = 'Luis',
	apellido = 'Terrazas',
	fecha_nacimiento = 11-08-1999;

INSERT INTO temperatura   
   SET idTemperatura    = 1,
       idPaciente   = (
       SELECT idPaciente  
         FROM paciente 
        WHERE idPaciente   = LAST_INSERT_ID()),
        idsensorTemperatura    = (
       SELECT idsensorTemperatura  
         FROM sensor_temperatura  
        WHERE idsensorTemperatura   = LAST_INSERT_ID()),
	fecha = CURRENT_DATE,
    hora = CURRENT_TIME,
    valor = 36;
    
INSERT INTO pulso   
   SET idPulso     = 1,
       idPaciente   = (
       SELECT idPaciente  
         FROM paciente 
        WHERE idPaciente   = LAST_INSERT_ID()),
        idsensorpulso_oxi     = (
       SELECT idsensorpulso_oxi   
         FROM sensor_pulso_oximetria   
        WHERE idsensorpulso_oxi    = LAST_INSERT_ID()),
	fecha = CURRENT_DATE,
    hora = CURRENT_TIME,
    valor = 80;
    
INSERT INTO oximetria   
   SET idOximetria      = 1,
       idPaciente   = (
       SELECT idPaciente  
         FROM paciente 
        WHERE idPaciente   = LAST_INSERT_ID()),
        idsensorpulso_oxi     = (
       SELECT idsensorpulso_oxi   
         FROM sensor_pulso_oximetria   
        WHERE idsensorpulso_oxi    = LAST_INSERT_ID()),
	fecha = CURRENT_DATE,
    hora = CURRENT_TIME,
    valor = 90.2;

