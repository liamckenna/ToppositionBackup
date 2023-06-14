#pragma once
#include <utility>
#include <string>
#include "SDL.h"
using namespace std;
class GameObject {
    string name;
    pair<float, float> position;
    pair<float, float> defaultPosition;
    pair<int, int> dimensions;
    pair<float, float> center;
    float originalRatio;
    float size;
    SDL_Texture* texture;
    SDL_Surface* surface;
    int layer;
    bool rendered = true;
    bool movable = false;
    SDL_Rect* renderRect = new SDL_Rect();
public:
    GameObject(string name, SDL_Texture* texture, SDL_Surface *surface, bool m, bool r, int l);
    pair<float, float> GetPosition(){return position;}
    pair<float, float> GetDimensions(){return dimensions;}
    string GetName(){return name;}
    SDL_Texture* GetTexture(){return texture;}
    SDL_Surface* GetSurface(){return surface;}
    bool GetRendered() {return rendered;}
    bool GetMovable() {return movable;}
    float GetSize() {return size;}
    SDL_Rect* GetRenderRect() {return renderRect;}
    pair<float, float> GetCenter() {return center;}
    void SetPosition(float x, float y, bool posOnly = false);
    void SetDefaultPosition(float x, float y);
    void SetCenter(float x = 0, float y = 0, bool centerOnly = false);
    void SetMovable(bool m);
    void SetRendered(bool r);
    void AdjustSize(float multiplier = 1, int w = 0, int h = 0);
    void RenderGameObject(SDL_Renderer* renderer);

};

