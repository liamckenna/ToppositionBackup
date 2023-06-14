#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <sys/types.h>
#include <dirent.h>
#include <vector>
#include <utility>
#include <map>
#include <math.h>
#include "GameObject.h"
#include "Input.h"

enum gameState {
    MAIN_MENU = 0,
    SETTINGS = 1,
    GAME = 2,
};
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
//The window we'll be rendering to
SDL_Window* gWindow = NULL;
//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;
//The window renderer
SDL_Renderer* gRenderer = NULL;
//Current displayed texture
SDL_Texture* gTexture = NULL;
std::map<std::string, SDL_Texture*> textures;
std::map<std::string, SDL_Surface*> surfaces;
GameObject* heldObject;
bool quit = false;

bool init();
bool loadMap(std::vector<std::vector<GameObject*>> &gameObjects);
void close();
SDL_Surface* loadSurface( std::string path );
SDL_Texture* loadTexture( std::string path );
std::map<std::string, SDL_Texture*> textureLoader();
void zoom(std::vector<std::vector<GameObject*>> gameObjects, SDL_Event e, Input* playerInput);
void scroll(std::vector<std::vector<GameObject*>> gameObjects, Input* playerInput);
void renderObjects(std::vector<std::vector<GameObject*>> gameObjects, SDL_Renderer* gRenderer);
GameObject* selectObject(std::vector<std::vector<GameObject*>> gameObjects, Input* playerInput);
void moveHeldObject(GameObject* gameObject, Input* playerInput);
SDL_Color GetPixelColor(const SDL_Surface* pSurface, const int X, const int Y);
void RecenterScreen(std::vector<std::vector<GameObject*>> gameObjects, Input* playerInput);
void HandleEvents(std::vector<std::vector<GameObject*>> gameObjects, Input* playerInput);
void RenderScreen(std::vector<std::vector<GameObject*>> gameObjects);

int main( int argc, char* args[] )
{
    //Start up SDL and create window
    if( !init() )
    {
        printf( "Failed to initialize!\n" );
    }
    else
    {

        std::vector<std::vector<GameObject*>> gameObjects;
        if( !loadMap(gameObjects) )
        {
            printf( "Failed to load media!\n" );
        }
        else
        {
            //Main loop flag


            //Event handler

            Input* playerInput = new Input();
            //While application is running


            while( !quit )
            {

                //Handle events on queue
                HandleEvents(gameObjects, playerInput);

                RenderScreen(gameObjects);

            }
        }
    }

    //Free resources and close SDL
    close();

    return 0;
}

bool init()
{
    //Initialization flag
    bool success = true;



    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        //Set texture filtering to linear
        if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
        {
            printf( "Warning: Linear texture filtering not enabled!" );
        }



        //Create window
        gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( gWindow == NULL )
        {
            printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
            /*Set Fullscreen
            if (SDL_SetWindowFullscreen(gWindow,SDL_WINDOW_FULLSCREEN_DESKTOP) < 0) {
                printf( "Warning: Fullscreen Failed! SDL Error: %s\n", SDL_GetError() );
            }*/
            //Create renderer for window
            gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
            if( gRenderer == NULL )
            {
                printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
                success = false;
            }
            else
            {
                //Initialize renderer color
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

                //Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;

                if( !( IMG_Init( imgFlags ) & imgFlags ) )
                {
                    printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
                    success = false;
                }
                else {
                    gScreenSurface = SDL_GetWindowSurface( gWindow );

                }
            }
        }
    }

    return success;
}

bool loadMap(std::vector<std::vector<GameObject*>> &gameObjects)
{
    textures = textureLoader();
    //Loading success flag
    bool success = true;

    //Load splash image

    GameObject* steve = new GameObject("steve", textures["steve"], surfaces["steve"], true, true, 1);
    GameObject* apple = new GameObject("apple", textures["apple"], surfaces["apple"], true, false, 1);
    GameObject* coin = new GameObject("coin", textures["coin"], surfaces["coin"], true, true, 1);
    GameObject* map = new GameObject("map", textures["map"], surfaces["map"], false, true, 0);
    GameObject* red = new GameObject("red", textures["red"], surfaces["red"], true, true, 1);



    vector<GameObject*> layer0;
    vector<GameObject*> layer1;
    vector<GameObject*> layer2;
    gameObjects.push_back(layer1);
    gameObjects.push_back(layer0);
    gameObjects.push_back(layer2);
    gameObjects[0].push_back(map);
    gameObjects[1].push_back(coin);
    gameObjects[1].push_back(steve);
    gameObjects[1].push_back(red);
    map->SetPosition(0, 0);
    coin->SetCenter(960, 540);
    steve->SetPosition(-100, 0);
    apple->SetPosition(1000, 1000);
    red->SetPosition(300, 300);

    return success;
}

void close()
{
    //Free loaded image
    for (auto i : textures) {
        SDL_DestroyTexture(i.second);
        i.second = NULL;
    }
    //Destroy window
    SDL_DestroyRenderer( gRenderer );
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;
    gRenderer = NULL;

    //Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}

SDL_Texture* loadTexture( std::string path )
{
    //The final texture
    SDL_Texture* newTexture = NULL;

    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
    if( loadedSurface == NULL )
    {
        printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
    }
    else
    {
        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
        if( newTexture == NULL )
        {
            printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
        }

        //Get rid of old loaded surface
        SDL_FreeSurface( loadedSurface );
    }

    return newTexture;
}

SDL_Surface* loadSurface( std::string path )
{
    //The final optimized image
    SDL_Surface* optimizedSurface = NULL;

    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
    if( loadedSurface == NULL )
    {
        printf( "Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
    }
    else
    {
        //Convert surface to screen format
        optimizedSurface = SDL_ConvertSurface( loadedSurface, gScreenSurface->format, 0 );
        if( optimizedSurface == NULL )
        {
            printf( "Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
        }

        //Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);

    }
    return optimizedSurface;
}

std::map<std::string, SDL_Texture*> textureLoader() {
    //load all textures
    DIR* dp;
    std::map<std::string, SDL_Texture*> textures;
    int i = 0;
    struct dirent* ep;
    dp = opendir("Images");
    if (dp != NULL) {
        while (ep = readdir(dp)) {
            if (ep->d_namlen == 1 || ep->d_namlen == 2) continue;
            else {
                std::string fileName = ep->d_name;
                std::size_t pos = fileName.find(".");
                std::string shorthand = fileName.substr(0,pos);
                std::string texturePath = "Images/" + fileName;
                textures[shorthand] = loadTexture(texturePath);
                surfaces[shorthand] = loadSurface(texturePath);
                i++;
            }

        }

        (void) closedir(dp);
    }
    else
        perror ("Couldn't open the directory");

    printf("There's %d files in the current directory.\n", i);
    return textures;
}

void zoom(std::vector<std::vector<GameObject*>> gameObjects, SDL_Event e, Input* playerInput) {

    std::pair<float, float> relativePositionB4;
    std::pair<float, float> newRelativePosition;
    SDL_GetMouseState(&playerInput->currentMousePosition.first, &playerInput->currentMousePosition.second);
    for (int i = 0; i < gameObjects.size(); i++) {
        for (int j = 0; j < gameObjects[i].size(); j++) {
            if (gameObjects[i][j]->GetRendered()) {
                std::cout << gameObjects[i][j]->GetSize() << std::endl;
                if (e.wheel.preciseY < 0 && gameObjects[i][j]->GetSize() <= 1) {

                    std::cout << "continuing..." << std::endl;
                    continue;
                } else {

                    std::cout << e.wheel.preciseY/abs(e.wheel.preciseY) << std::endl;
                    relativePositionB4.first = (gameObjects[i][j]->GetPosition().first - playerInput->currentMousePosition.first)  / gameObjects[i][j]->GetSize();
                    relativePositionB4.second = (gameObjects[i][j]->GetPosition().second - playerInput->currentMousePosition.second)  / gameObjects[i][j]->GetSize();
                    gameObjects[i][j]->AdjustSize((e.wheel.preciseY/abs(e.wheel.preciseY) * .1));
                    newRelativePosition.first = (gameObjects[i][j]->GetPosition().first - playerInput->currentMousePosition.first)  / gameObjects[i][j]->GetSize();
                    newRelativePosition.second = (gameObjects[i][j]->GetPosition().second - playerInput->currentMousePosition.second)  / gameObjects[i][j]->GetSize();

                    gameObjects[i][j]->SetCenter((gameObjects[i][j]->GetCenter().first + (relativePositionB4.first - newRelativePosition.first) * gameObjects[i][j]->GetSize()),
                                                 (gameObjects[i][j]->GetCenter().second + (relativePositionB4.second - newRelativePosition.second) * gameObjects[i][j]->GetSize()));
                    if (gameObjects[i][j]->GetSize() == 1 && i > 0) {
                        gameObjects[i][j]->SetDefaultPosition(gameObjects[i][j]->GetPosition().first, gameObjects[i][j]->GetPosition().second);
                    }
                }

            }

        }
    }
    RecenterScreen(gameObjects, playerInput);
}

void renderObjects(std::vector<std::vector<GameObject*>> gameObjects, SDL_Renderer* gRenderer) {
    for (int i = 0; i < gameObjects.size(); i++) {
        for (int j = 0; j < gameObjects[i].size(); j++) {
            if (gameObjects[i][j]->GetRendered()) {
                gameObjects[i][j]->RenderGameObject(gRenderer);
            }
        }
    }

}

void scroll(std::vector<std::vector<GameObject*>> gameObjects, Input* playerInput) {
    bool enabled = true;
    for (int i = 0; i < gameObjects.size(); i++) {
        for (int j = 0; j < gameObjects[i].size(); j++) {
            if (i == 0) {
                if (gameObjects[i][j]->GetSize() <= 1 ||
                    (gameObjects[i][j]->GetPosition().first > 0 && ((playerInput->currentMousePosition.first - playerInput->prevMousePosition.first) > 0)) ||
                    ((gameObjects[i][j]->GetPosition().first + (gameObjects[i][j]->GetSize() * gameObjects[i][j]->GetDimensions().first)) < SCREEN_WIDTH && ((playerInput->currentMousePosition.first - playerInput->prevMousePosition.first) < 0)) ||
                    (gameObjects[i][j]->GetPosition().second > 0 && ((playerInput->currentMousePosition.second - playerInput->prevMousePosition.second) > 0)) ||
                    ((gameObjects[i][j]->GetPosition().second + (gameObjects[i][j]->GetSize() * gameObjects[i][j]->GetDimensions().second)) < SCREEN_HEIGHT && ((playerInput->currentMousePosition.second - playerInput->prevMousePosition.second) < 0))) {
                    enabled = false;
                    continue;
                } else {
                    enabled = true;
                }
            }
            if (enabled){
                gameObjects[i][j]->SetPosition(gameObjects[i][j]->GetPosition().first + playerInput->currentMousePosition.first - playerInput->prevMousePosition.first,
                                               gameObjects[i][j]->GetPosition().second + playerInput->currentMousePosition.second - playerInput->prevMousePosition.second);
            }
        }
    }
    RecenterScreen(gameObjects, playerInput);
}

GameObject* selectObject(std::vector<std::vector<GameObject*>> gameObjects, Input* playerInput) {
    SDL_GetMouseState(&playerInput->currentMousePosition.first, &playerInput->currentMousePosition.second);
    //std::cout << playerInput->currentMousePosition.first << ", " << playerInput->currentMousePosition.second << endl;
    for (int i = gameObjects.size(); i > 0; i--) {
        for (int j = 0; j < gameObjects[i].size(); j++) {
            int width_LowerBound = gameObjects[i][j]->GetPosition().first;
            int width_UpperBound = gameObjects[i][j]->GetPosition().first + (gameObjects[i][j]->GetDimensions().first * gameObjects[i][j]->GetSize());
            int height_LowerBound = gameObjects[i][j]->GetPosition().second;
            int height_UpperBound = gameObjects[i][j]->GetPosition().second + (gameObjects[i][j]->GetDimensions().second * gameObjects[i][j]->GetSize());
            /*if (gameObjects[i][j]->GetName() == "coin") {
                std::cout << width_LowerBound << ", " << width_UpperBound << endl;
                std::cout << height_LowerBound << ", " << height_UpperBound << endl;
            }*/

            if (playerInput->currentMousePosition.first >= width_LowerBound && playerInput->currentMousePosition.first <= width_UpperBound) {
                if (playerInput->currentMousePosition.second >= height_LowerBound && playerInput->currentMousePosition.second <= height_UpperBound) {
                    //std::cout << playerInput->currentMousePosition.first - width_LowerBound << ", " << playerInput->currentMousePosition.second - height_LowerBound << std::endl;

                    //std::cout << playerInput->currentMousePosition.first - width_LowerBound << std::endl;
                    SDL_Color color = GetPixelColor(gameObjects[i][j]->GetSurface(),
                                                    (playerInput->currentMousePosition.first - width_LowerBound)/gameObjects[i][j]->GetSize(),
                                                    (playerInput->currentMousePosition.second - height_LowerBound)/gameObjects[i][j]->GetSize());
                    if (color.r == 0 && color.g == 0 && color.b == 0) {
                        continue;
                    } else if (gameObjects[i][j]->GetMovable()){
                        return gameObjects[i][j];
                    }


                }
            }

        }
    }
    return nullptr;
}

SDL_Color GetPixelColor(const SDL_Surface* pSurface, const int X, const int Y)
{
    // Bytes per pixel
    const Uint8 Bpp = pSurface->format->BytesPerPixel;

    /*
    Retrieve the address to a specific pixel
    pSurface->pixels	= an array containing the SDL_Surface' pixels
    pSurface->pitch		= the length of a row of pixels (in bytes)
    X and Y				= the offset on where on the image to retrieve the pixel, (0, 0) is in the upper left corner of the image
    */
    Uint8* pPixel = (Uint8*)pSurface->pixels + Y * pSurface->pitch + X * Bpp;

    Uint32 PixelData = *(Uint32*)pPixel;

    SDL_Color Color = {0x00, 0x00, 0x00};

    // Retrieve the RGB values of the specific pixel
    SDL_GetRGBA(PixelData, pSurface->format, &Color.r, &Color.g, &Color.b, &Color.a);
    return Color;
}

void moveHeldObject(GameObject* gameObject, Input* playerInput) {
    gameObject->SetPosition(gameObject->GetPosition().first + playerInput->currentMousePosition.first - playerInput->prevMousePosition.first,
                                   gameObject->GetPosition().second + playerInput->currentMousePosition.second - playerInput->prevMousePosition.second);
}

void RecenterScreen(std::vector<std::vector<GameObject*>> gameObjects, Input* playerInput) {
    if (gameObjects[0][0]->GetPosition().first > 0) {
        for (int i = gameObjects.size() - 1; i >= 0; i--) {
            for (int j = gameObjects[i].size() - 1; j >= 0; j--) {
                gameObjects[i][j]->SetPosition(gameObjects[i][j]->GetPosition().first - gameObjects[0][0]->GetPosition().first,
                                               gameObjects[i][j]->GetPosition().second);
            }
        }
    }
    if (gameObjects[0][0]->GetPosition().first + gameObjects[0][0]->GetDimensions().first * gameObjects[0][0]->GetSize() < SCREEN_WIDTH) {
        for (int i = gameObjects.size() - 1; i >= 0; i--) {
            for (int j = gameObjects[i].size() - 1; j >= 0; j--) {
                gameObjects[i][j]->SetPosition(gameObjects[i][j]->GetPosition().first + (SCREEN_WIDTH - (gameObjects[0][0]->GetPosition().first + gameObjects[0][0]->GetDimensions().first * gameObjects[0][0]->GetSize())),
                                               gameObjects[i][j]->GetPosition().second);
            }
        }
    }
    if (gameObjects[0][0]->GetPosition().second > 0) {
        for (int i = gameObjects.size() - 1; i >= 0; i--) {
            for (int j = gameObjects[i].size() - 1; j >= 0; j--) {
                gameObjects[i][j]->SetPosition(gameObjects[i][j]->GetPosition().first,
                                               gameObjects[i][j]->GetPosition().second - gameObjects[0][0]->GetPosition().second);
            }
        }
    }
    if (gameObjects[0][0]->GetPosition().second + gameObjects[0][0]->GetDimensions().second * gameObjects[0][0]->GetSize() < SCREEN_HEIGHT) {
        for (int i = gameObjects.size() - 1; i >= 0; i--) {
            for (int j = gameObjects[i].size() - 1; j >= 0; j--) {
                gameObjects[i][j]->SetPosition(gameObjects[i][j]->GetPosition().first,
                                               gameObjects[i][j]->GetPosition().second + (SCREEN_HEIGHT - (gameObjects[0][0]->GetPosition().second + gameObjects[0][0]->GetDimensions().second * gameObjects[0][0]->GetSize())));
            }
        }
    }
}

void HandleEvents(std::vector<std::vector<GameObject*>> gameObjects, Input* playerInput) {
    SDL_Event e;
    SDL_GetMouseState(&playerInput->currentMousePosition.first, &playerInput->currentMousePosition.second);
    while( SDL_PollEvent( &e ) != 0 )
    {
        switch(e.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_MOUSEWHEEL:
                if (e.wheel.preciseY > 0 || e.wheel.preciseY < 0) {
                    zoom(gameObjects, e, playerInput);
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                playerInput->MouseButtonDown(e.button);
                if (playerInput->GetMouseButtonDown("Left")) {
                    heldObject = selectObject(gameObjects, playerInput);
                }
                break;
            case SDL_MOUSEBUTTONUP:
                playerInput->MouseButtonUp(e.button);
                heldObject = nullptr;
                break;
        }
    }
    SDL_GetMouseState(&playerInput->currentMousePosition.first, &playerInput->currentMousePosition.second);
    if (playerInput->GetMouseButtonDown("Middle")) {
        scroll(gameObjects, playerInput);
    }
    if (playerInput->GetMouseButtonDown("Left")) {
        if (heldObject != nullptr) {
            moveHeldObject(heldObject, playerInput);
        }
    }
    SDL_GetMouseState(&playerInput->prevMousePosition.first, &playerInput->prevMousePosition.second);
}

void RenderScreen(std::vector<std::vector<GameObject*>> gameObjects){
    SDL_SetRenderDrawColor( gRenderer, 255, 255, 255, 100 );
    SDL_RenderClear( gRenderer );

    //Render texture to screen
    renderObjects(gameObjects, gRenderer);
    SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0x00, 0xFF );

    //Update screen
    SDL_RenderPresent( gRenderer);
}