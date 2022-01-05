#include <bits/stdc++.h>
#include <vector>
#include <forward_list>
using namespace std;

string GetString(fstream *file)
{
    string str;
    while (true)
    {
        if (file->eof())
        {
            break;
        }
        string temp;
        getline(*file, temp);
        str += temp;
    }
    return str;
}

int main()
{

    fstream f1;
    fstream f2;
    f1.open("f31.txt", ios::in | ios::out);
    f2.open("f32.txt", ios::out);

    if (f1)
    {
        string str;
        f1 << "I am 21 years old. My height is 5ft. 11 inches.";
    }
    else
    {
        cout << "file f1 does not exists, quitting";
    }
    // doing this to reset the read pos
    f1.seekg(0);

    string str = GetString(&f1);
    string num;
    string text;
    for (int i = 0; i < str.size(); i++)
    {
        if (str[i] != ' ')
        {
            if (str[i] >= 48 && str[i] <= 57)
            {
                num.push_back(str[i]);
            }
            else
            {
                text.push_back(str[i]);
            }
        }
    }
    f2 << text;
    f2 << num;

    f1.close();
    f2.close();
}
