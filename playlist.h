#ifndef PLAYLIST_H
#define PLAYLIST_H

#include "data.h"
#include "queue.h"
#include "filehandler.h"
#include <fstream>

class PlaylistManager
{
private:
    // Node untuk Lagu di dalam Playlist
    struct SongNode
    {
        Song song;
        SongNode *next;
        SongNode(Song s) : song(s), next(nullptr) {}
    };

    // Node untuk Nama Playlist
    struct PlaylistNode
    {
        string name;
        SongNode *headSong; // Head ini adalah TOP of STACK
        PlaylistNode *next;
        PlaylistNode(string n) : name(n), headSong(nullptr), next(nullptr) {}
    };

    PlaylistNode *headPL;

public:
    PlaylistManager() : headPL(nullptr) {}

    // Buat Playlist Baru
    void create(string name)
    {
        // Cek apakah nama sudah ada
        PlaylistNode *temp = headPL;
        while (temp)
        {
            if (temp->name == name)
            {
                cout << "Playlist sudah ada!" << endl;
                return;
            }
            temp = temp->next;
        }

        PlaylistNode *newNode = new PlaylistNode(name);
        newNode->next = headPL; // Insert di depan
        headPL = newNode;
        cout << "Playlist '" << name << "' berhasil dibuat." << endl;
    }

    // Tambah Lagu (Konsep Stack: Insert at Head)
    void addSong(string plName, Song song)
    {
        PlaylistNode *temp = headPL;
        while (temp)
        {
            if (temp->name == plName)
            {
                SongNode *sNode = new SongNode(song);
                sNode->next = temp->headSong; // Push ke Head
                temp->headSong = sNode;
                cout << "Lagu ditambahkan ke '" << plName << "' (Posisi Teratas)." << endl;
                return;
            }
            temp = temp->next;
        }
        cout << "Playlist tidak ditemukan." << endl;
    }

    // Masukkan ke Queue Player
    void playPlaylist(string plName, MusicQueue &player)
    {
        PlaylistNode *temp = headPL;
        while (temp)
        {
            if (temp->name == plName)
            {
                if (!temp->headSong)
                {
                    cout << "Playlist kosong!" << endl;
                    return;
                }

                player.clear(); // Bersihkan queue lama
                cout << "Memuat playlist ke queue..." << endl;

                // Masukkan dari Head (Lagu terakhir ditambahkan masuk queue duluan)
                SongNode *s = temp->headSong;
                while (s)
                {
                    player.enqueue(s->song);
                    s = s->next;
                }
                player.playCurrent(); // Mainkan langsung
                return;
            }
            temp = temp->next;
        }
        cout << "Playlist tidak ditemukan!" << endl;
    }

    void display()
    {
        PlaylistNode *temp = headPL;
        cout << "\n=== DAFTAR PLAYLIST ===" << endl;
        if (!headPL)
            cout << "(Belum ada playlist)" << endl;

        while (temp)
        {
            cout << "- " << temp->name << endl;
            temp = temp->next;
        }
    }

    // Save Format: NamaPlaylist|Judul|Artis|Durasi|Path
    void save(string nik)
    {
        ofstream file(nik + "_playlist.txt");
        PlaylistNode *p = headPL;
        while (p)
        {
            SongNode *s = p->headSong;
            while (s)
            {
                file << p->name << "|"
                     << s->song.title << "|"
                     << s->song.artist << "|"
                     << s->song.duration << "|"
                     << s->song.filePath << endl;
                s = s->next;
            }
            p = p->next;
        }
        file.close();
    }

    // Load Manual
    void load(string nik)
    {
        ifstream file(nik + "_playlist.txt");
        if (!file.is_open())
            return;

        // Kita perlu array buffer agar urutan insert stacknya benar
        // Karena struktur file: LaguBaru -> LaguLama (sesuai save loop)
        // Saat load, kita baca dari bawah ke atas agar "LaguLama" masuk dulu, baru "LaguBaru" di atasnya

        string lines[200]; // Buffer baris
        int totalLines = 0;
        string line;

        while (getline(file, line) && totalLines < 200)
        {
            lines[totalLines++] = line;
        }
        file.close();

        string parts[10];
        int c;

        // Loop Terbalik dari bawah ke atas
        for (int i = totalLines - 1; i >= 0; i--)
        {
            FileHandler::split(lines[i], '|', parts, c);
            if (c == 5)
            {
                string plName = parts[0];

                // Cek playlist sudah ada belum, kalau belum create
                bool exists = false;
                PlaylistNode *check = headPL;
                while (check)
                {
                    if (check->name == plName)
                        exists = true;
                    check = check->next;
                }

                if (!exists)
                    create(plName);

                // Add song
                addSong(plName, {parts[1], parts[2], stoi(parts[3]), parts[4]});
            }
        }
    }
};

#endif