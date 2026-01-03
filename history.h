#ifndef HISTORY_H
#define HISTORY_H

#include "data.h"
#include "filehandler.h"
#include <fstream>

class HistoryManager
{
private:
    struct Node
    {
        Song song;
        Node *next;
        Node(Song s) : song(s), next(nullptr) {}
    };
    Node *top; // Puncak Stack

public:
    HistoryManager() : top(nullptr) {}

    // Push: Tambah ke paling atas
    void push(Song song)
    {
        // Cek agar lagu yang sama tidak menumpuk berurutan di history
        if (top && top->song.title == song.title)
            return;

        Node *newNode = new Node(song);
        newNode->next = top;
        top = newNode;
    }

    void display()
    {
        if (!top)
        {
            cout << "History Kosong." << endl;
            return;
        }
        Node *temp = top;
        cout << "\n=== HISTORY (Terakhir Diputar) ===" << endl;
        while (temp)
        {
            cout << "- " << temp->song.title << " (" << temp->song.artist << ")" << endl;
            temp = temp->next;
        }
    }

    // Save: Simpan dari Top ke Bawah
    void save(string nik)
    {
        ofstream file(nik + "_history.txt");
        Node *temp = top;
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

    // Load: Baca file dan masukkan kembali ke Stack
    void load(string nik)
    {
        ifstream file(nik + "_history.txt");
        if (!file.is_open())
            return;

        // Kita butuh array sementara agar urutan tidak terbalik saat di-push ulang
        // Asumsi maksimal history yang disimpan 100 terakhir
        string lines[100];
        int totalLines = 0;
        string line;

        // 1. Baca semua baris ke array dulu
        while (getline(file, line) && totalLines < 100)
        {
            lines[totalLines++] = line;
        }
        file.close();

        // 2. Masukkan ke Stack dari baris TERAKHIR (Bawah) ke AWAL (Atas)
        // Agar saat di-push, yang baris pertama file kembali menjadi Top
        string parts[10];
        int c;
        for (int i = totalLines - 1; i >= 0; i--)
        {
            FileHandler::split(lines[i], '|', parts, c);
            if (c == 4)
            {
                push({parts[0], parts[1], stoi(parts[2]), parts[3]});
            }
        }
    }
};

#endif