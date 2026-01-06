#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <string>
#include <sstream>

using namespace std;

class FileHandler
{
public:
    // Menerima array string 'elems' sebagai tempat menampung hasil
    // 'count' akan diisi jumlah potongan kata yang didapat
    static void split(const string &s, char delim, string elems[], int &count)
    {
        stringstream ss(s);
        string item;
        count = 0;

        // Loop ambil kata per delimiter
        while (getline(ss, item, delim))
        {
            elems[count] = item;
            count++;
        }
    }
};

#endif