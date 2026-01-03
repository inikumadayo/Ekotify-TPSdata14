#include <iostream>
#include "data.h"
#include "ktp.h"
#include "musichandler.h"
#include "history.h"
#include "queue.h"
#include "playlist.h"
#include "favorite.h"

using namespace std;

// Inisialisasi Global Var
Song *globalCurrentSong = nullptr;
KTP *KTPSekarang = nullptr;

int main()
{
    // 1. Load Data Awal
    MusicHandler musicLib;
    musicLib.loadMusic("listMusic.txt"); // Pastikan file ini ada

    KTPManager userSystem;
    userSystem.loadUsers();

    // 2. Sistem Login
    bool loggedIn = false;
    while (!loggedIn)
    {
        system("cls");
        cout << "\n=== EKOTIFY START ===" << endl;
        cout << "1. Login" << endl;
        cout << "2. Register" << endl;
        cout << "0. Exit" << endl;
        cout << ">> ";
        int c;
        if (!(cin >> c))
        {
            cin.clear();
            cin.ignore(1000, '\n');
            c = -1;
        }

        if (c == 1)
            loggedIn = userSystem.login();
        else if (c == 2)
        {
            userSystem.registerUser();
            system("pause");
        }
        else
            return 0;
    }

    // 3. Load User Data (Playlist/Fav/History)
    HistoryManager history;
    history.load(KTPSekarang->NIK);

    FavoriteManager fav;
    fav.load(KTPSekarang->NIK);

    PlaylistManager playlist;
    playlist.load(KTPSekarang->NIK);

    MusicQueue player(&history);

    // 4. Main Menu Loop
    bool running = true;
    while (running)
    {
        system("cls");
        cout << "\n=== DASHBOARD (" << KTPSekarang->nama << ") ===" << endl;

        // Status Bar
        if (globalCurrentSong)
            cout << "[PLAYING]: " << globalCurrentSong->title << " - " << globalCurrentSong->artist << endl;
        else
            cout << "[IDLE] Tidak ada lagu diputar." << endl;

        cout << "-----------------------------------" << endl;
        cout << "1. Lihat Daftar Lagu (Library)" << endl; // <--- FITUR BARU
        cout << "2. Cari & Putar Lagu" << endl;
        cout << "3. Player Controls" << endl;
        cout << "4. Playlist Manager" << endl;
        cout << "5. Favorites" << endl;
        cout << "6. History" << endl;
        cout << "0. Save & Logout" << endl;
        cout << ">> ";

        int m;
        if (!(cin >> m))
        {
            cin.clear();
            cin.ignore(1000, '\n');
            m = -1;
        }

        if (m == 1)
        {
            // MENAMPILKAN SEMUA LAGU DARI AVL TREE
            musicLib.displayAll();
            cout << "\n(Tekan enter untuk kembali...)" << endl;
            system("pause");
        }
        else if (m == 2)
        {
            cout << "\nMasukkan Judul Lagu (Case Sensitive): ";
            cin.ignore();
            string t;
            getline(cin, t);

            Song *s = musicLib.searchSong(t);

            if (s)
            {
                cout << "\n[DITEMUKAN]: " << s->title << " - " << s->artist << endl;
                cout << "[1] Play Now | [2] Add Queue | [3] Add Fav | [0] Cancel: ";
                int act;
                cin >> act;

                if (act == 1)
                {
                    // --- PERBAIKAN DI SINI ---
                    // Memanggil fungsi playImmediate yang baru kita buat
                    player.playImmediate(*s);
                    // -------------------------
                }
                else if (act == 2)
                {
                    player.enqueue(*s);
                }
                else if (act == 3)
                {
                    fav.add(*s);
                }
            }
            else
            {
                cout << "[ERROR] Lagu tidak ditemukan." << endl;
            }
            system("pause");
        }
        else if (m == 3)
        {
            cout << "\n=== PLAYER CONTROLS ===" << endl;
            cout << "[1] Play Current Queue\n[2] Pause\n[3] Resume\n[4] Stop\n[5] Next Song\n[6] Prev Song\n[7] Show Queue\n>> ";
            int pc;
            cin >> pc;
            if (pc == 1)
                player.playCurrent();
            else if (pc == 2)
                player.pause();
            else if (pc == 3)
                player.resume();
            else if (pc == 4)
                player.stop();
            else if (pc == 5)
                player.next();
            else if (pc == 6)
                player.prev();
            else if (pc == 7)
                player.display();
            system("pause");
        }
        else if (m == 4)
        {
            cout << "\n=== PLAYLIST MANAGER ===" << endl;
            cout << "1. Buat Playlist Baru\n2. Tambah Lagu ke Playlist\n3. Putar Playlist\n4. Lihat Daftar Playlist\n>> ";
            int pl;
            cin >> pl;
            if (pl == 1)
            {
                cout << "Nama Playlist Baru: ";
                cin.ignore();
                string n;
                getline(cin, n);
                playlist.create(n);
            }
            else if (pl == 2)
            {
                playlist.display();
                cout << "Masukkan Nama Playlist: ";
                cin.ignore();
                string n;
                getline(cin, n);
                cout << "Masukkan Judul Lagu: ";
                string t;
                getline(cin, t);

                Song *s = musicLib.searchSong(t);
                if (s)
                    playlist.addSong(n, *s);
                else
                    cout << "Lagu tidak ada di library." << endl;
            }
            else if (pl == 3)
            {
                playlist.display();
                cout << "Masukkan Nama Playlist: ";
                cin.ignore();
                string n;
                getline(cin, n);
                playlist.playPlaylist(n, player);
            }
            else if (pl == 4)
            {
                playlist.display();
            }
            system("pause");
        }
        else if (m == 5)
        {
            fav.display();
            cout << "\n[1] Putar Lagu Favorit (by Nomor) | [0] Back: ";
            int fc;
            cin >> fc;
            if (fc == 1)
            {
                cout << "Nomor urut: ";
                int no;
                cin >> no;
                Song *s = fav.getSongByIndex(no);
                if (s)
                {
                    player.enqueue(*s);
                    cout << "Dimasukkan ke Queue." << endl;
                }
                else
                    cout << "Nomor salah." << endl;
            }
        }
        else if (m == 6)
        {
            history.display();
            system("pause");
        }
        else if (m == 0)
        {
            // Save semua data user sebelum keluar
            cout << "Menyimpan data..." << endl;
            history.save(KTPSekarang->NIK);
            fav.save(KTPSekarang->NIK);
            playlist.save(KTPSekarang->NIK);
            player.stop();
            running = false;
            cout << "Logout Berhasil. Sampai Jumpa!" << endl;
        }
    }

    return 0;
}