#include "../imports.h"

using namespace std;

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

vector<PlayerData> LoadFile(string filePath)
{
    ifstream myFile(filePath);

    if (!myFile.is_open())
    {
        cout << "Could not read from file" << endl;
        char buffer[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, buffer);
        cout << "Tried to open: " << buffer << endl;
    }

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
