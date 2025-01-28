/*This source code copyrighted by Lazy Foo' Productions 2004-2025
and may not be redistributed without written permission.*/

/* Headers */
//Using SDL, SDL_image, and STL string
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <string>


/* Constants */

constexpr float ESCALA { 2.f };
constexpr int ESCALA_i { 2 };

//Screen dimension constants
constexpr int kScreenWidth{ 768 * ESCALA_i };
constexpr int kScreenHeight{ 192 * ESCALA_i };

/* Class Prototypes */
class LTexture
{
public:
    //Symbolic constant
    static constexpr float kOriginalSize = -1.f;

    //Initializes texture variables
    LTexture();

    //Cleans up texture variables
    ~LTexture();

    //Loads texture from disk
    bool loadFromFile( std::string path, bool esSprite = false);

    //Cleans up texture
    void destroy();

    //Draws texture
    void render( float x, float y, SDL_FRect* clip = nullptr, float width = kOriginalSize, float height = kOriginalSize,
                 double degrees = 0.0, SDL_FPoint* center = nullptr, SDL_FlipMode flipMode = SDL_FLIP_NONE);

    //Gets texture dimensions
    int getWidth();
    int getHeight();

private:
    //Contains texture data
    SDL_Texture* mTexture;

    //Texture dimensions
    int mWidth;
    int mHeight;
};


/* Function Prototypes */
//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();


/* Global Variables */
//The window we'll be rendering to
SDL_Window* gWindow{ nullptr };

//The renderer used to draw to the window
SDL_Renderer* gRenderer{ nullptr };

//The sprite sheet texture
LTexture gSpriteSheetTexture; 

/// The background
LTexture gBackgroundTexture; 

/* Class Implementations */
//LTexture Implementation
LTexture::LTexture():
    //Initialize texture variables
    mTexture{ nullptr },
    mWidth{ 0 },
    mHeight{ 0 }
{

}

LTexture::~LTexture()
{
    //Clean up texture
    destroy();
}

bool LTexture::loadFromFile( std::string path, bool esSprite )
{
    //Clean up texture if it already exists
    destroy();

    //Load surface
    if( SDL_Surface* loadedSurface = IMG_Load( path.c_str() ); loadedSurface == nullptr )
    {
        SDL_Log( "Unable to load image %s! SDL_image error: %s\n", path.c_str(), SDL_GetError() );
    }
    else
    {
        if (esSprite)
        {
            // Color key image
            if (!SDL_SetSurfaceColorKey(loadedSurface, true, SDL_MapSurfaceRGB(loadedSurface, 0x74, 0x74, 0x74)))
            {
                SDL_Log("Unable to color key! SDL error: %s", SDL_GetError());
            }
        }

        // Create texture from surface
        mTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if (mTexture == nullptr)
        {
            SDL_Log("Unable to create texture from loaded pixels! SDL error: %s\n", SDL_GetError());
        }
        else
        {
            // Get image dimensions
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }
        
        //Clean up loaded surface
        SDL_DestroySurface( loadedSurface );
    }

    //Return success if texture loaded
    return mTexture != nullptr;
}

void LTexture::destroy()
{
    //Clean up texture
    SDL_DestroyTexture( mTexture );
    mTexture = nullptr;
    mWidth = 0;
    mHeight = 0;
}

void LTexture::render( float x, float y, SDL_FRect* clip, float width, float height,
                       double degrees, SDL_FPoint* center, SDL_FlipMode flipMode )
{
    //Set texture position
    SDL_FRect dstRect = { x, y, static_cast<float>( mWidth ), static_cast<float>( mHeight ) };

    //Default to clip dimensions if clip is given
    if( clip != nullptr )
    {
        dstRect.w = clip->w;
        dstRect.h = clip->h;
    }

    //Resize if new dimensions are given
    if( width > 0 )
    {
        dstRect.w = width;
    }
    if( height > 0 )
    {
        dstRect.h = height;
    }

    // Renderizamos en <gRenderer>. Recortamos de mTexutre el rectángulo <clip>, lo pintamos en <dstRect>,
    // rotado <degrees> grados alrededor de <center> y con <flipMode>
    SDL_RenderTextureRotated( gRenderer, mTexture, clip, &dstRect, degrees, center, flipMode );
}

int LTexture::getWidth()
{
    return mWidth;
}

int LTexture::getHeight()
{
    return mHeight;
}


/* Function Implementations */
bool init()
{
    //Initialization flag
    bool success{ true };

    //Initialize SDL
    if( !SDL_Init( SDL_INIT_VIDEO ) )
    {
        SDL_Log( "SDL could not initialize! SDL error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        //Create window with renderer
        if( !SDL_CreateWindowAndRenderer( "SDL3 Tutorial: Sprite Clipping and Stretching", kScreenWidth, kScreenHeight, 0, &gWindow, &gRenderer ) )
        {
            SDL_Log( "Window could not be created! SDL error: %s\n", SDL_GetError() );
            success = false;
        }
    }

    return success;
}

bool loadMedia()
{
    //File loading flag
    bool success{ true };

    //Load scene images
    if( success &= gSpriteSheetTexture.loadFromFile( "assets/Simon.png", true); !success )
    {
        SDL_Log( "Unable to load sprite sheet image!\n");
    }
    if( success &= gBackgroundTexture.loadFromFile( "assets/Castlevania_lvl1.png" ); !success )
    {
        SDL_Log( "Unable to load background image!\n");
    }

    return success;
}

void close()
{
    //Clean up textures
    gSpriteSheetTexture.destroy();

    gBackgroundTexture.destroy();
    
    //Destroy window
    SDL_DestroyRenderer( gRenderer );
    gRenderer = nullptr;
    SDL_DestroyWindow( gWindow );
    gWindow = nullptr;

    //Quit SDL subsystems
    SDL_Quit();
}

int main( int argc, char* args[] )
{
    //Final exit code
    int exitCode{ 0 };

    //Initialize
    if( !init() )
    {
        SDL_Log( "Unable to initialize program!\n" );
        exitCode = 1;
    }
    else
    {
        //Load media
        if( !loadMedia() )
        {
            SDL_Log( "Unable to load media!\n" );
            exitCode = 2;
        }
        else
        {
            //The quit flag
            bool quit{ false };

            //The event data
            SDL_Event e;
            SDL_zero( e );

            //Rotation degrees
            double degrees = 0.0;

            //Flipmode
            SDL_FlipMode flipMode = SDL_FLIP_NONE;

            //The main loop
            while( quit == false )
            {
                //Get event data
                while( SDL_PollEvent( &e ) )
                {
                    //If event is quit type
                    if( e.type == SDL_EVENT_QUIT )
                    {
                        //End the main loop
                        quit = true;
                    }
                    //On key press
                    else if( e.type == SDL_EVENT_KEY_DOWN )
                    {
                        switch( e.key.key )
                        {
                            // left/right press
                            case SDLK_LEFT:
                                flipMode = SDL_FLIP_NONE;
                                break;
                            case SDLK_RIGHT:
                                flipMode = SDL_FLIP_HORIZONTAL;
                                break;
                            default:
                                break;
                        }
                    }
                }

                // Set rotation degrees on keyboard state
                const bool* keyStates = SDL_GetKeyboardState( nullptr );
                if( keyStates[ SDL_SCANCODE_UP ] )
                {
                    switch (flipMode) {
                        case SDL_FLIP_HORIZONTAL:
                            degrees = 270;
                            break;
                        case SDL_FLIP_NONE:
                            degrees = 90;
                            break;
                        default:
                            break;
                    }
                }
                else if( keyStates[ SDL_SCANCODE_DOWN ] )
                {
                    switch (flipMode) {
                        case SDL_FLIP_HORIZONTAL:
                            degrees = 90;
                            break;
                        case SDL_FLIP_NONE:
                            degrees = 270;
                            break;
                        default:
                            break;
                    }
                }
                else
                {
                    degrees = 0;
                }


                //Fill the background white
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                SDL_RenderClear( gRenderer );


                // -------- Background texture --------

                //Init background clip
                constexpr float kBgSizeH = 192.f;       // pensaba que eran 182 pero son 192, wtf
                constexpr float kBgSizeW = 768.f;
                SDL_FRect bgClip = { 0.f, 0.f, kBgSizeW, kBgSizeH };
                
                //Init background size
                SDL_FRect bgSize = { 0.f, 0.f, kBgSizeW, kBgSizeH };

                
                //Use top left sprite
                bgClip.x = 1.f;
                bgClip.y = 11.f;

                //Set sprite size
                bgSize.w = kBgSizeW * ESCALA;
                bgSize.h = kBgSizeH * ESCALA;

                // Draw background
                gBackgroundTexture.render(0.f, 0.f, &bgClip, bgSize.w, bgSize.h );


                // -------- Simon --------

                //Init sprite clip
                constexpr float kSpriteSizeH = 32.f;
                constexpr float kSpriteSizeW = 16.f;
                SDL_FRect spriteClip = { 0.f, 0.f, kSpriteSizeW, kSpriteSizeH };
                
                //Init sprite size
                SDL_FRect spriteSize = { 0.f, 0.f, kSpriteSizeW, kSpriteSizeH };
                
                //Use top left sprite
                spriteClip.x = 1.f;
                spriteClip.y = 21.f;

                //Set sprite size
                spriteSize.w = kSpriteSizeW * ESCALA;
                spriteSize.h = kSpriteSizeH * ESCALA;

                //Define center of rotation of Simon sprite
                SDL_FPoint center = { spriteSize.w / 2.f, spriteSize.h / 2.f };

                //Draw sprite
                gSpriteSheetTexture.render(245.f * ESCALA - spriteSize.w, 169.f * ESCALA - spriteSize.h,
                                            &spriteClip, spriteSize.w, spriteSize.h, degrees, &center, flipMode);




                //Update screen
                SDL_RenderPresent( gRenderer );
            } 
        }
    }

    //Clean up
    close();

    return exitCode;
}
