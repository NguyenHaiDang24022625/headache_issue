#include "Map.h" //5
#include "Game.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "ECS/ECS.h" 
#include "ECS/Components.h"

using namespace std;

extern Manager manager; // sử dụng tiếp manager đã được khai báo ở Game.cpp

Map::Map(string tID, int mapscale, int tilesize){
    texID = tID;
    mapScale = mapscale;
    tileSize = tilesize;
}

Map::~Map() = default;

void Map::LoadMap(string path, int sizeX, int sizeY, int gridWidth, int offsetY) {
    fstream mapFile(path, ios::in);
    if (!mapFile.is_open()) {
        cerr << "Failed to open map file: " << path << std::endl;
        return;
    }

    string line;
    vector<vector<int>> mapData;

    // Đọc dữ liệu từ file và lưu vào vector 2D
    while (getline(mapFile, line)) {
        istringstream ss(line); // 
        vector<int> row;
        string value;

        while (getline(ss, value, ',')) {
            row.push_back(stoi(value)); // Chuyển giá trị từ chuỗi sang số nguyên
        }

        mapData.push_back(row);
    }

    mapFile.close();

    // Duyệt qua dữ liệu bản đồ và thêm các tile
    for (int y = 0; y < sizeY; ++y) {
        for (int x = 0; x < sizeX / 2; ++x) {
            int tileCode = mapData[y][x];
            int srcX = tileCode % gridWidth; // Cột trong sprite sheet
            int srcY = tileCode / gridWidth; // Hàng trong sprite sheet

            AddTile(srcX * tileSize, srcY * tileSize, x * (tileSize * mapScale), y * (tileSize * mapScale) + offsetY ); // Thêm tile vào bản đồ
            
        }
    }

    // Duyệt qua bản đồ thêm các collider
    for (int y = sizeY; y < sizeY * 2; ++y) {
        for (int x = 0; x < sizeX / 2; ++x) {
            int tileCode = mapData[y][x];
            if (tileCode == 1){
                Entity& block = manager.addEntity();
                block.addComponent<ColliderComponent>("terrain", x * (tileSize * mapScale), (y - sizeY) * (tileSize * mapScale) + offsetY, tileSize * mapScale);
                block.addGroup(Game::groupColliders);

            }
            else if (tileCode == 2){
                Entity& block = manager.addEntity();
                block.addComponent<ColliderComponent>("terrain", x * (tileSize * mapScale), (y - sizeY) * (tileSize * mapScale) + offsetY, tileSize * mapScale);
                block.addGroup(Game::groupDangers);
                // cout << "Danger created at: " 
                //      << x * (tileSize * mapScale) << ", " 
                //      << (y - sizeY) * (tileSize * mapScale) + offsetY << endl;
            }
        }
    }
}

void Map::AddTile(int srcX, int srcY, int xpos, int ypos){ //18
    Entity& Map = manager.addEntity(); //13
    Map.addComponent<TileComponent>(srcX, srcY, xpos, ypos, tileSize, mapScale, texID);
    Map.addGroup(Game::groupMap);
}

void Map::loadVehiclesForMap(int mapIndex) {
    
    // Đọc file phương tiện mới
    fstream file(Game::vehiclesFiles[mapIndex], ios::in);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            istringstream iss(line);
            int x, y, w, h, scale, veloX, veloY;
            string texID;
            
            iss >> x >> y >> w >> h >> scale >> veloX >> veloY;
            iss >> texID;
            
            // Điều chỉnh tọa độ y cho map2 nếu cần
            y -= Game::Mapcounter * 1024; // offset giống như map
            
            Entity& vehicle = manager.addEntity();
            vehicle.addComponent<TransformComponent>(x, y, w, h, scale, veloX, veloY);
            vehicle.addComponent<SpriteComponent>(texID);
            vehicle.addComponent<ColliderComponent>(texID);
            vehicle.addGroup(Game::groupVehicles);
        }
        
        file.close();
    }
}
