#include "module.h"

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
