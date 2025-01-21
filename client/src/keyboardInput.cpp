#include "../include/keyboardInput.h"

void split_str(string line, char sign, vector<string> lineArgs)
{
    int i = 0;
    string args = "";
    for (int j = 0; j < line.length(); j++)
    {
        if(line.at(j) == sign)
        {
            lineArgs.at(i) = args;
            i += 1;
        }
        else
        args += line.at(j);
    }
    lineArgs.at(i) = args;
}