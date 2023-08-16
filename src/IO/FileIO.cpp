#include "../globals.h"
#include "../imports.h"
#include "../utils.h"

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
        Log(logger,
            "Did not find file: " + path +
                "\n  Search root: " + string(buffer));
    }
}

Tilemap LoadMap(string filePath)
{
    ifstream mapFile(filePath);
    CheckFile(mapFile, filePath);

    int rows, columns;
    int mapValue;

    if (!(mapFile >> rows >> columns))
    {
        Log(logger,
            "Invalid file format. Expecting row column definition at the "
            "beginning!");
        return Tilemap{0, 0};
    }

    int* values = new int[rows * columns];
    int* currentValue = values;

    for (int y = 0; y < rows; y++)
    {
        for (int x = 0; x < columns; x++)
        {
            if (!(mapFile >> mapValue))
            {
                char text[256];
                sprintf(text,
                        "Expected to find value for (%d,%d), but was "
                        "missing in the map! Expecting %d rows and %d columns",
                        x,
                        y,
                        rows,
                        columns);
                Log(logger, text);
            }

            *currentValue++ = mapValue;
        }
    }

    Tilemap map = {rows, columns};
    map.AssignMap(values);
    return map;
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
