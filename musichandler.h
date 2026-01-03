#ifndef MUSICHANDLER_H
#define MUSICHANDLER_H

#include "data.h"
#include "filehandler.h"
#include <fstream>

class MusicHandler
{
private:
    struct Node
    {
        Song song;
        Node *left, *right;
        int height;
        Node(Song s) : song(s), left(nullptr), right(nullptr), height(1) {}
    };

    Node *root;
    Song *allSongsArray; // Array manual
    int totalSongs;

    // --- FUNGSI HELPER MANUAL ---

    // Fungsi Max Manual (Pengganti std::max)
    int maxVal(int a, int b)
    {
        return (a > b) ? a : b;
    }

    int getHeight(Node *N) { return N ? N->height : 0; }

    int getBalance(Node *N)
    {
        if (!N)
            return 0;
        return getHeight(N->left) - getHeight(N->right); // Normal Logic (Kiri - Kanan)
    }

    Node *rightRotate(Node *y)
    {
        Node *x = y->left;
        Node *T2 = x->right;
        x->right = y;
        y->left = T2;

        // Pakai maxVal manual
        y->height = maxVal(getHeight(y->left), getHeight(y->right)) + 1;
        x->height = maxVal(getHeight(x->left), getHeight(x->right)) + 1;
        return x;
    }

    Node *leftRotate(Node *x)
    {
        Node *y = x->right;
        Node *T2 = y->left;
        y->left = x;
        x->right = T2;

        // Pakai maxVal manual
        x->height = maxVal(getHeight(x->left), getHeight(x->right)) + 1;
        y->height = maxVal(getHeight(y->left), getHeight(y->right)) + 1;
        return y;
    }

    Node *insert(Node *node, Song song)
    {
        if (!node)
            return new Node(song);
        if (song.title < node->song.title)
            node->left = insert(node->left, song);
        else if (song.title > node->song.title)
            node->right = insert(node->right, song);
        else
            return node;

        node->height = 1 + maxVal(getHeight(node->left), getHeight(node->right));
        int balance = getBalance(node);

        // Rotasi Normal (LL, RR, LR, RL)
        if (balance > 1 && song.title < node->left->song.title)
            return rightRotate(node);
        if (balance < -1 && song.title > node->right->song.title)
            return leftRotate(node);
        if (balance > 1 && song.title > node->left->song.title)
        {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }
        if (balance < -1 && song.title < node->right->song.title)
        {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }
        return node;
    }

    Song *searchRec(Node *node, string title)
    {
        if (!node || node->song.title == title)
            return node ? &node->song : nullptr;
        if (title < node->song.title)
            return searchRec(node->left, title);
        return searchRec(node->right, title);
    }

    void displayInOrder(Node *node)
    {
        if (node)
        {
            displayInOrder(node->left);
            cout << "- " << node->song.title << " (" << node->song.artist << ")" << endl;
            displayInOrder(node->right);
        }
    }

public:
    MusicHandler() : root(nullptr), totalSongs(0)
    {
        allSongsArray = new Song[1000]; // Alokasi array manual
    }

    // PENTING: Destructor untuk menghapus array manual agar tidak memory leak
    ~MusicHandler()
    {
        delete[] allSongsArray;
    }

    void loadMusic(string filename)
    {
        ifstream file(filename);
        if (!file.is_open())
            return;

        string line;
        string parts[10]; // Array statis untuk menampung hasil split
        int c;

        while (getline(file, line))
        {
            // Panggil split manual
            FileHandler::split(line, '|', parts, c);

            if (c == 4)
            { // Pastikan ada 4 bagian (Judul|Artis|Durasi|Path)
                Song s = {parts[0], parts[1], stoi(parts[2]), parts[3]};
                root = insert(root, s);
                allSongsArray[totalSongs++] = s;
            }
        }
        file.close();
    }

    Song *searchSong(string title)
    {
        return searchRec(root, title);
    }

    void displayAll()
    {
        cout << "\n=== LIBRARY LAGU (A-Z) ===" << endl;
        displayInOrder(root);
    }
};

#endif