#include "module.h"
#include <sstream>

struct PlayerData
{
    int id;
    string name;
    float money;

    PlayerData(int id, string name, float money)
    {
        this->id = id;
        this->name = name;
        this->money = money;
    }
};

void CheckFile(ifstream& file, string path)
{
    if (!file.is_open())
    {
        char buffer[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, buffer);
        Log("Did not find file: " + path +
            "\n  Search root: " + string(buffer));
    }
}

TileMapRaw LoadMap(string filePath)
{
    ifstream mapFile(filePath);
    CheckFile(mapFile, filePath);

    int rows, columns;
    char mapValue;

    if (!(mapFile >> rows >> columns))
    {
        Log("Invalid file format. Expecting row column definition at the "
            "beginning!");

        return TileMapRaw{};
    }

    char* values = new char[rows * columns];
    char* currentValue = values;

    for (int y = 0; y < rows; y++)
    {
        for (int x = 0; x < columns; x++)
        {
            if (!(mapFile >> mapValue))
            {
                Logf("Expected to find value for (%d,%d), but was missing in "
                     "the map! Expecting %d rows and %d columns",
                     x,
                     y,
                     rows,
                     columns);
            }

            *currentValue++ = mapValue;
        }
    }

    return TileMapRaw{true, rows, columns, values};
}

ItemData LoadItem(string filePath)
{
    map<string, string> fileMap;
    ifstream item(filePath);

    if (item.is_open())
    {

        string line;
        while (getline(item, line))
        {
            if (line.empty() || line[0] == '#') continue;

            istringstream streamLine(line);
            string key;
            string value;

            if (getline(streamLine, key, '='))
            {
                if (getline(streamLine, value))
                {
                    fileMap[key] = value;
                }
            }
        }
        item.close();
    }
    else
    {
        Logf("Unable to open file %s", filePath.c_str());
        return ItemData{};
    }

    Logf("Parsing values for file %s", filePath.c_str());

    // FIXME: doesn't ignore whitespace
    // Crashes then becaues can't parse it
    ItemData data = {};
    data.weight = stof(fileMap["weight"]);
    data.aero = stof(fileMap["aero"]);
    data.power = stof(fileMap["power"]);
    data.stability = stof(fileMap["stability"]);
    data.sturdieness = stof(fileMap["sturdieness"]);

    data.effect_types = stoi(fileMap["effects"], 0, 2);
    data.hit_sound = fileMap["hit_sound"];
    data.shoot_sound = fileMap["shoot_sound"];
    data.bullet_sprite_idx = stoi(fileMap["bullet_sprite"]);
    data.pillar_sprite_idx = stoi(fileMap["pillar_sprite"]);

    data.loaded = true;

    return data;
}
ItemArray LoadItems(string directoryPath)
{
    WIN32_FIND_DATA fileData;

    string searchPath = directoryPath + "\\*.*";
    HANDLE hFind = FindFirstFile(searchPath.c_str(), &fileData);

    vector<string> files;

    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
            string fileName = fileData.cFileName;
            files.push_back(fileName);
        }
        while (FindNextFile(hFind, &fileData) != 0);

        FindClose(hFind);
    }
    else
    {
        Logf("Error finding files while searching directory %s",
             directoryPath.c_str());
    }

    int itemCount = files.size();
    Logf("Detected %d item files in items directory", itemCount);
    ItemData* items = new ItemData[itemCount];

    int index = 0;
    for (string file : files)
    {
        items[index++] = LoadItem(directoryPath + "\\" + file);
    }

    return ItemArray{items, itemCount};
}

vector<PlayerData> LoadFile(string filePath)
{
    ifstream myFile(filePath);
    CheckFile(myFile, filePath);

    int id;
    string name;
    float money;
    vector<PlayerData> players = {};

    while (myFile >> id >> name >> money)
    {
        players.push_back(PlayerData(id, name, money));
    }

    return players;
}

void AppendToFile(string filePath, vector<PlayerData> players)
{
    fstream appendingFile;
    appendingFile.open("outputtest.txt", ios::app);

    if (!appendingFile.is_open())
    {
        cout << "Could not open output file" << endl;
    }

    for (PlayerData cur : players)
    {
        appendingFile << cur.id << " " << cur.name << " " << cur.money << endl;
    }

    appendingFile.close();
}

void Test_SaveLoadFileIO()
{
    // loads from the execution directory
    vector<PlayerData> players = LoadFile("test.txt");
    AppendToFile("outputtest.txt", players);
}
