
<?php

    try {
        $pdo = new PDO("mysql:host=localhost;dbname=estacion_6",'root', '');
        echo "Conexion exitosa a la base de datos";
    }   catch(PDOExceptions $e){
        echo "<h1>Error conectando a la base de datos</h1>";
        print 'Error, ' . $e->getMessage();
        die();
    }



?>