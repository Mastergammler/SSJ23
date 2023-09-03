#include "module.h"

string SelectFilePath()
{
    ShowCursor(true);

    OPENFILENAME ofn;
    TCHAR szFile[MAX_PATH] = {0};

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = TEXT("Select map to open");
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

    if (GetOpenFileName(&ofn))
    {
        Logf("Opening file: %s", szFile);
    }
    else
    {
        DWORD error = CommDlgExtendedError();
        if (error != 0)
            Logf("Error: %s", error);
        else
            Log("No file selected");
    }

    ShowCursor(false);
    return szFile;
}
