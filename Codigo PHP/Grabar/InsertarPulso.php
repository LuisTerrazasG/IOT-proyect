<?php

    include 'conexiones.php';
    if($_GET){
        $idPaciente = $_GET["idPaciente"];
        $idsensorpulso_oxi = $_GET["idsensorpulso_oxi"];
        $valor = $_GET["valor"];
        
        $sql_agregar = "INSERT INTO pulso (idPaciente,idsensorpulso_oxi,valor)
                VALUES (?,?,?)";
        $sentencia_agregar = $pdo->prepare($sql_agregar);
        $resultado = $sentencia_agregar->execute(array($idPaciente,$idsensorpulso_oxi,$valor));

        if($resultado==true) {
            $sentencia_agregar = null;
            $pdo = null;
            echo "\nSe insertarion los datos de una manera correcta";
        } else{
            echo"Error al insertar en la BD";
        }
    }
    else{
        echo"Faltan los datos";
    }

?>