# Videojuegos by Eduardo Mena

Creando un videojuego 2D divertido que tenga a Eduardo Mena como boss final.

## Lista de Tareas

| Tarea | Descripción | Responsables | Estado |
|-------|-------------|--------------|--------|
| Plantear Idea            | --- | Todos        | En progreso  |
| Diseño de arquitectura   | --- | Todos        | Sin comenzar |
| Arte del juego           | --- | Juan         | Sin comenzar |

## Primeros pasos del repositorio para ejecutar el juego

1. Hacer "pull"
2. Ejecutar "make clean" (de momento solo va en Windows).
3. Ejecutar comando "make". CUIDADO: si no se han seguido los pasos del anexo 1, hay que modificar rutas de las líneas 3 y 4 del MAKEFILE haciendo caso a los comentarios
4. Ejecutar comando "game.exe"

## ANEXO 1: Instalación desde 0 de SDL3 3.20 en Windows (25/01/2025)

1. Crear el proyecto en una ruta sin nombres raros (espacios, signos raros, etc...). La carpeta del repositorio del Github Desktop sirve.
2. Descargar del repositorio de SDL3 versión 3.20: SDL3-devel-3.2.0-mingw.zip (para tener la librería) y SDL3-3.2.0-win32-x64.zip en Windows (para tener el sdl3.dll)
3. Descomprimir el primer zip en el directorio del proyecto. Llamemos a la carpeta descomprimida "SDL3".
4. Ahora, queremos quedarnos solo con los ficheros relevantes. Para ello, borraremos todos los ficheros y directorios excepto el contenido de la carpeta SDL3/x86_64-w64-mingw32/. Finalmente, el contenido de SDL3/x86_64-w64-mingw32/ lo volcamos en SDL3/ y borramos la carpeta x86_64-w64-mingw32/
5. En cuanto al segundo zip, colocamos el "sdl3.dll" al lado del "main.cpp" del proyecto (de momento por conveniencia).
6. Si se ha hecho todo bien, ya podemos ejecutar el Makefile del repositorio del proyecto y lanzar el ejecutable game.exe
