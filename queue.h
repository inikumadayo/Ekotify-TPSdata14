#ifndef QUEUE_H
#define QUEUE_H

#include "data.h"
#include "history.h"
#include <windows.h>
#include <mmsystem.h>

class MusicQueue
{
private:
    struct Node
    {
        Song song;
        Node *next, *prev;
        Node(Song s) : song(s), next(nullptr), prev(nullptr) {}
    };
    Node *head, *tail, *current;
    HistoryManager *historyRef;

    bool isPlaying = false;
    bool isPaused = false;

public:
    MusicQueue(HistoryManager *h) : head(nullptr), tail(nullptr), current(nullptr), historyRef(h) {}

    void enqueue(Song song)
    {
        Node *newNode = new Node(song);
        if (!head)
        {
            head = tail = current = newNode;
        }
        else
        {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
        cout << "[QUEUE] Menambahkan: " << song.title << endl;
    }

    // --- [FITUR BARU: PLAY NOW] ---
    void playImmediate(Song song)
    {
        // 1. Masukkan lagu ke antrian paling belakang
        enqueue(song);

        // 2. Paksa pointer 'current' lompat ke lagu yang barusan dimasukkan (tail)
        current = tail;

        // 3. Mainkan langsung
        cout << "[PLAYER] Memotong antrian untuk memutar lagu ini..." << endl;
        playCurrent();
    }
    // -----------------------------

    void clear()
    {
        stop();
        Node *temp = head;
        while (temp)
        {
            Node *del = temp;
            temp = temp->next;
            delete del;
        }
        head = tail = current = nullptr;
    }

    void playCurrent()
    {
        if (current)
        {
            stop(); // Stop lagu sebelumnya
            globalCurrentSong = &current->song;
            historyRef->push(current->song);

            string cmd = "open \"" + current->song.filePath + "\" type mpegvideo alias ekotifyPlayer";
            mciSendStringA(cmd.c_str(), NULL, 0, NULL);
            mciSendStringA("play ekotifyPlayer", NULL, 0, NULL);
            isPlaying = true;
            isPaused = false;

            cout << "NOW PLAYING: " << current->song.title << endl;
        }
        else
        {
            cout << "Tidak ada lagu yang dipilih / Antrian habis." << endl;
        }
    }

    void next()
    {
        if (current && current->next)
        {
            current = current->next;
            playCurrent();
        }
        else
            cout << "Sudah di akhir antrian." << endl;
    }

    void prev()
    {
        if (current && current->prev)
        {
            current = current->prev;
            playCurrent();
        }
        else
            cout << "Sudah di awal antrian." << endl;
    }

    void pause()
    {
        if (isPlaying && !isPaused)
        {
            mciSendStringA("pause ekotifyPlayer", NULL, 0, NULL);
            isPaused = true;
            cout << "Paused." << endl;
        }
    }

    void resume()
    {
        if (isPlaying && isPaused)
        {
            mciSendStringA("resume ekotifyPlayer", NULL, 0, NULL);
            isPaused = false;
            cout << "Resumed." << endl;
        }
    }

    void stop()
    {
        mciSendStringA("stop ekotifyPlayer", NULL, 0, NULL);
        mciSendStringA("close ekotifyPlayer", NULL, 0, NULL);
        isPlaying = false;
    }

    void display()
    {
        Node *temp = head;
        int i = 1;
        cout << "\n=== QUEUE (FIFO) ===" << endl;
        if (!head)
            cout << "(Antrian Kosong)" << endl;
        while (temp)
        {
            cout << i++ << ". " << temp->song.title << (temp == current ? " [PLAYING]" : "") << endl;
            temp = temp->next;
        }
    }
};

#endif