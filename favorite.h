#ifndef FAVORITE_H
#define FAVORITE_H

#include "data.h"
#include "filehandler.h"
#include <fstream>

class FavoriteManager
{
private:
    struct Node
    {
        Song song;
        Node *next, *prev;
        Node(Song s) : song(s), next(nullptr), prev(nullptr) {}
    };
    Node *head;

public:
    FavoriteManager() : head(nullptr) {}

    // Tambah ke Head (Agar O(1) / Cepat)
    void add(Song song)
    {
        // Cek duplikat
        Node *check = head;
        while (check)
        {
            if (check->song.title == song.title)
            {
                cout << "Lagu sudah ada di Favorite." << endl;
                return;
            }
            check = check->next;
        }

        Node *newNode = new Node(song);
        if (head)
        {
            newNode->next = head;
            head->prev = newNode;
        }
        head = newNode;
        cout << "[FAV] Ditambahkan: " << song.title << endl;
    }

    void display()
    {
        if (!head)
        {
            cout << "Favorite Kosong." << endl;
            return;
        }
        cout << "\n=== FAVORITE SONGS ===" << endl;
        Node *temp = head;
        int i = 1;
        while (temp)
        {
            cout << i++ << ". " << temp->song.title << " - " << temp->song.artist << endl;
            temp = temp->next;
        }
    }

    // Ambil lagu berdasarkan nomor urut (untuk diputar)
    Song *getSongByIndex(int index)
    {
        Node *temp = head;
        int count = 1;
        while (temp)
        {
            if (count == index)
                return &temp->song;
            count++;
            temp = temp->next;
        }
        return nullptr;
    }

    void save(string nik)
    {
        ofstream file(nik + "_fav.txt");
        Node *temp = head;
        while (temp)
        {
            file << temp->song.title << "|"
                 << temp->song.artist << "|"
                 << temp->song.duration << "|"
                 << temp->song.filePath << endl;
            temp = temp->next;
        }
        file.close();
    }

    void load(string nik)
    {
        ifstream file(nik + "_fav.txt");
        if (!file.is_open())
            return;

        string lines[100];
        int totalLines = 0;
        string line;

        // Baca ke array dulu
        while (getline(file, line) && totalLines < 100)
        {
            lines[totalLines++] = line;
        }
        file.close();

        // Loop terbalik agar urutan insert kembali benar
        string parts[10];
        int c;
        for (int i = totalLines - 1; i >= 0; i--)
        {
            FileHandler::split(lines[i], '|', parts, c);
            if (c == 4)
            {
                add({parts[0], parts[1], stoi(parts[2]), parts[3]});
            }
        }
    }
};

#endif