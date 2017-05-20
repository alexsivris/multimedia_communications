#ifndef TEXTFILE_H
#define TEXTFILE_H
#include "connection.h"
using namespace std;

class TextFile : public Connection
{
public:
    TextFile(int &socket);
private:
    void sendText();
    void writeFile();

    fstream m_fs;
    string m_text;
};

#endif // TEXTFILE_H
