#ifndef DATA_H
#define DATA_H

#include <string>
#include <iostream>

using namespace std;

// Struktur Data Lagu
struct Song
{
    string title;
    string artist;
    int duration;
    string filePath;
};

// Struktur Data User
struct KTP
{
    string NIK;
    string nama;
    string alamat;
    string tanggalLahir;
    bool isAdmin; // Field untuk menandai admin
};

// Variabel Global (Extern)
extern Song *globalCurrentSong;
extern KTP *KTPSekarang;

#endif