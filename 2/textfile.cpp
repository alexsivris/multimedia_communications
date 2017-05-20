#include "textfile.h"

TextFile::TextFile(int &socket)
    : Connection(socket)
{

    sendText();
}

void TextFile::sendText()
{
    char * send_buf = "TEXT";
    char recv_buf[1500];
    int retcode = 0;

    retcode = send(m_socket,send_buf,sizeof(send_buf),0);
    if (retcode > 0)
    {
        retcode = recv(m_socket,recv_buf,sizeof(recv_buf),0);
        while (strcmp(recv_buf,"END"))
        {
            cout << recv_buf ;
            m_text.append(recv_buf);
            retcode = recv(m_socket,recv_buf,sizeof(recv_buf),0);
        }
        writeFile();
    }

}

void TextFile::writeFile()
{
    ifstream ifile("TextFile.txt");
    if (ifile)
        remove("TextFile.txt");
    m_fs.open("TextFile.txt",fstream::in | fstream::out | fstream::app);
    m_fs << m_text;
    cout << "wrote file!"<<endl;

}
