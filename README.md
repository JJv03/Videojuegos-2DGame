# Videojuegos by Eduardo Mena

Creando un videojuego 2D divertido que tenga a Eduardo Mena como boss final.

## Lista de Tareas

| Tarea | Descripción | Responsables | Estado |
|-------|-------------|--------------|--------|
| Plantear Idea            | --- | Todos        | En progreso  |
| Diseño de arquitectura   | --- | Todos        | Sin comenzar |
| Arte del juego           | --- | Juan         | Sin comenzar |


## Instalación de SDL3

1. Crear el proyecto en una ruta sin nombres raros (espacios, signos raros, etc...)
2. Descargar del repositorio original SDL3 versión 3.20: SDL3-3.2.0.zip (para tener la librería) y SDL3-3.2.0-win32-x64.zip en Windows (para tener el sdl3.dll)
3. Descomprimir el primer zip en el directorio del proyecto. Llamemos a la carpeta descomprimida "SDL3".
4. Crear una carpeta en ".../SDL3" llamada "build".
5. Colocar el "sdl3.dll" al lado del "main.cpp" del proyecto.
6. En la ruta ".../mi_proyecto/SDL3/build/" ejecutar "cmake .. -DSDL_STATIC=OFF"
7. Al finalizarse, ejecutar "ninja".
8. Vamos al directorio raiz del proyecto (".../mi_proyecto/") y ejecutar: g++ main.cpp -o main -I./SDL3/include -L./SDL3/build -lSDL3
9. Abrimos el ejecutable con: main.exe
