class FileHandler
{
public:
    string fileName;
    fstream file;
    FileHandler(string x)
    {
        fileName = x;
        file.open(x, ios::in | ios::out);
    }

    string Read(string delim)
    {
        if (!file)
        {
            return "got nothing";
        }
        string str;
        while (true)
        {
            string temp;
            getline(file, temp);
            str += delim;
            str += temp;
            if (file.eof())
            {
                break;
            }
        }
        return str;
    }
    void Write(string str)
    {
        if (file)
        {
            file << str;
        }
    }

    void Close()
    {
        if (file)
        {
            file.close();
        }
    }
};