Para generar los archivos utilizados para el entrenamiento:
    1. Situarse en la carpeta raíz del proyecto.
    2. Situar en esta los archivos x.csv (RAM) e y.csv (teclas), pues los usará el paso siguiente.
    3. Ejecutar: python3 preprocess_dataset.py
    4. Esto genera 3 pares de  archivos:
        4.1. X_train.csv, y_train.csv
        4.2. X_validation.csv, y_validation.csv
        4.3. X_test.csv, y_test.csv

Para compilar el main_ale.cpp, que es el main para entrenar un bot:
    1. Situarse en la carpeta raíz del proyecto.
    2. Ejecutar el comando: make ale
    3. Generará el ejecutable main_ale, el cual podemos ejecutar: ./main_ale
    4. Su ejecución dará como resultado 2 archivos:
        4.1. history.csv, que contiene el historial de errores del entrenamiento del bot.
        4.2. ale_bot.csv, que contiene los datos del bot para que pueda ser cargado y usado posteriormente.

Una vez tenemos un bot:
    1. Situamos la carpeta de ALE como un subdirectorio de nuestro proyecto.
    2. Nos situamos en esta carpeta de ALE.
    3. Ponemos en la carpeta de ALE el minimal_agent.cpp.    
    4. Ejecutamos: make
    5. El comando generará el ejecutable ale
    6. Para ejecutar el programa: ./C19RETRO.robot <ruta a la rom assault.bin>

