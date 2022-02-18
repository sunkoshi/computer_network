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
    f1.open("f21.txt", ios::in | ios::out);
    f2.open("f22.txt", ios::out);

    if (f1)
    {
        string str;
        f1 << "hello world";
    }
    else
    {
        cout << "file f1 does not exists, quitting";
    }
    // doing this to reset the read pos
    f1.seekg(0);

    string str = GetString(&f1);
    for (int i = 0; i < str.size(); i++)
    {
        str[i] = toupper(str[i]);
    }
    f2 << str;

    f1.close();
    f2.close();
}
