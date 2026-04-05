#include "leveldata.h"

std::vector<Texture2D> solidTilesTextures = {};

void LoadAssets()
{
    solidTilesTextures.clear();

    //solid tiles

    solidTilesTextures.push_back(LoadTexture("assets/tiles/solid/top-left-outter-corner.png")); 

    solidTilesTextures.push_back(LoadTexture("assets/tiles/solid/floor.png"));

    solidTilesTextures.push_back(LoadTexture("assets/tiles/solid/top-right-outter-corner.png"));

    solidTilesTextures.push_back(LoadTexture("assets/tiles/solid/wall-right.png"));

    solidTilesTextures.push_back(LoadTexture("assets/tiles/solid/bottom-left-outter-corner.png"));

    solidTilesTextures.push_back(LoadTexture("assets/tiles/solid/ceiling.png"));
    
    solidTilesTextures.push_back(LoadTexture("assets/tiles/solid/bottom-right-outter-corner.png"));

    solidTilesTextures.push_back(LoadTexture("assets/tiles/solid/wall-left.png"));
}

void UnloadAssets()
{
    solidTilesTextures.clear();
}
