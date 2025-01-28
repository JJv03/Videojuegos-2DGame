#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

// Variables para controlar el rectángulo
static float rect_x = 500.0f;           // Posición inicial en X
static float rect_y = 500.0f;           // Posición inicial en Y
static const float rect_width = 50.0f;  // Ancho del rectángulo
static const float rect_height = 50.0f; // Alto del rectángulo
static const float rect_speed = 0.05f;  // Velocidad de movimiento

// Esta función se ejecuta una vez al iniciar.
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    // Crear la ventana
    if (!SDL_CreateWindowAndRenderer("Game", 800, 600, SDL_WINDOW_FULLSCREEN, &window, &renderer))
    {
        SDL_Log("No se pudo crear la ventana y el renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    return SDL_APP_CONTINUE;
}

// Esta función se ejecuta cuando ocurre un evento nuevo (entrada de teclado, ratón, etc).
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT || event->key.scancode == SDL_SCANCODE_ESCAPE)
    {
        return SDL_APP_SUCCESS; // Finalizar el programa
    }

    return SDL_APP_CONTINUE;
}

// Esta función se ejecuta una vez por frame.
SDL_AppResult SDL_AppIterate(void *appstate)
{
    // Actualizar eventos
    SDL_PumpEvents();

    // Obtener el estado del teclado
    const bool *keyboard_state = SDL_GetKeyboardState(NULL);

    // HABRÍA QUE AÑADIR UNA PEQUEÑA ACELERACIÓN AL PRINCIPIO DEL MOVIMIENTO (empezar lento hasta una velocidad base)
    // ADEMÁS AÑADIR UN EFECTO DE DESLIZAMIENTO CUANDO SE CAMBIE DE DIRECCIÓN DE MOVIMIENTO
    // Actualizar la posición del rectángulo según las teclas presionadas
    if (keyboard_state[SDL_SCANCODE_A] || keyboard_state[SDL_SCANCODE_LEFT])
    {
        rect_x -= rect_speed;
    }
    if (keyboard_state[SDL_SCANCODE_D] || keyboard_state[SDL_SCANCODE_RIGHT])
    {
        rect_x += rect_speed;
    }

    // Limpiar la pantalla
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Negro
    SDL_RenderClear(renderer);

    // Dibujar el rectángulo
    SDL_FRect rect = {rect_x - rect_width / 2, rect_y - rect_height / 2, rect_width, rect_height};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Blanco
    SDL_RenderFillRect(renderer, &rect);

    // Actualizar la pantalla
    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

// Esta función se ejecuta al finalizar.
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
}
