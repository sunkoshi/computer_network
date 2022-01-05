#include <bits/stdc++.h>
#include <vector>
#include <forward_list>
using namespace std;

vector<int> GetNumbersFromFile(fstream *file)
{
    string str;
    vector<int> arr;
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
    for (int i = 0; i < str.size(); i++)
    {
        if (str[i] != ' ')
        {
            // is a number
            int a = int(str[i]) - 48;
            arr.push_back(a);
        }
    }
    return arr;
}

int main()
{

    fstream f1;
    fstream f2;
    fstream f3;
    f1.open("f1.txt", ios::in | ios::out);
    f2.open("f2.txt", ios::in | ios::out);
    f3.open("f3.txt", ios::out);

    if (f1)
    {
        string str;
        f1 << "1 2 5 7";
    }
    else
    {
        cout << "file f1 does not exists, quitting";
    }
    if (f2)
    {
        string str;
        f2 << "3 6 9";
    }
    else
    {
        cout << "file f2 does not exists, quitting";
    }

    // doing this to reset the read pos
    f1.seekg(0);
    f2.seekg(0);

    vector<int> set1 = GetNumbersFromFile(&f1);
    vector<int> set2 = GetNumbersFromFile(&f2);
    for (int x : set2)
    {
        set1.push_back(x);
    }
    sort(&set1[0], &set1[set1.size()]);
    string str;
    for (int x : set1)
    {
        string temp = to_string(x);
        str += temp;
        str += " ";
    }
    f3 << str;

    f1.close();
    f2.close();
    f3.close();
}
