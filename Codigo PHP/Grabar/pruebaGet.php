<?php
    if ($_GET){
        $numero = $_GET["numero"];
        $apellido = $_GET["apellido"];
        $nombre = $_GET["nombre"];
        echo "<h1> Datos recibidos por el medotod GET";
        echo "<br>Numero: " . $numero;
        echo "<br>Apellido: " . $apellido;
        echo "<br>Nombre: " . $nombre;
    } else {
        echo "No hay datos recibidos de GET";
    }


?>