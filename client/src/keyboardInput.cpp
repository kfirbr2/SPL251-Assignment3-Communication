#include "../include/KeyBoardInput.h"
#include <vector>
#include <string>

void split_str(std::string line, char sign, std::vector<std::string>& lineArgs)
{
    std::string word = "";
    for (auto x : line)
    {
        if (x == sign)
        {
            lineArgs.push_back(word);
            word = "";
        }
        else
        {
            word = word + x;
        }
    }
    lineArgs.push_back(word);
}
