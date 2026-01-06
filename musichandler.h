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
        if (!node)
            return nullptr;
        if (node->song.title == title)
            return &node->song;
        if (title < node->song.title)
            return searchRec(node->left, title);
        return searchRec(node->right, title);
    }

    Node *minValueNode(Node *node)
    {
        Node *current = node;
        while (current->left != nullptr)
            current = current->left;
        return current;
    }

    Node *deleteNode(Node *root, string title)
    {
        if (root == nullptr)
            return root;

        if (title < root->song.title)
            root->left = deleteNode(root->left, title);
        else if (title > root->song.title)
            root->right = deleteNode(root->right, title);
        else
        {
            // Node dengan 1 atau 0 child
            if ((root->left == nullptr) || (root->right == nullptr))
            {
                Node *temp = root->left ? root->left : root->right;
                if (temp == nullptr)
                {
                    temp = root;
                    root = nullptr;
                }
                else
                    *root = *temp;
                delete temp;
            }
            else
            {
                // Node dengan 2 children
                Node *temp = minValueNode(root->right);
                root->song = temp->song;
                root->right = deleteNode(root->right, temp->song.title);
            }
        }

        if (root == nullptr)
            return root;

        // Update height
        root->height = 1 + maxVal(getHeight(root->left), getHeight(root->right));

        // Balance tree
        int balance = getBalance(root);

        // Left Left
        if (balance > 1 && getBalance(root->left) >= 0)
            return rightRotate(root);

        // Left Right
        if (balance > 1 && getBalance(root->left) < 0)
        {
            root->left = leftRotate(root->left);
            return rightRotate(root);
        }

        // Right Right
        if (balance < -1 && getBalance(root->right) <= 0)
            return leftRotate(root);

        // Right Left
        if (balance < -1 && getBalance(root->right) > 0)
        {
            root->right = rightRotate(root->right);
            return leftRotate(root);
        }

        return root;
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

    // Fungsi untuk Admin: Menambah lagu baru
    void addSong(string title, string artist, int duration, string filePath)
    {
        Song s = {title, artist, duration, filePath};
        root = insert(root, s);
        allSongsArray[totalSongs++] = s;
        cout << "Lagu berhasil ditambahkan!" << endl;
    }

    // Fungsi untuk Admin: Menghapus lagu
    bool deleteSong(string title)
    {
        Song *s = searchSong(title);
        if (s)
        {
            root = deleteNode(root, title);

            // Hapus dari array juga
            for (int i = 0; i < totalSongs; i++)
            {
                if (allSongsArray[i].title == title)
                {
                    // Shift elemen
                    for (int j = i; j < totalSongs - 1; j++)
                    {
                        allSongsArray[j] = allSongsArray[j + 1];
                    }
                    totalSongs--;
                    break;
                }
            }

            cout << "Lagu berhasil dihapus!" << endl;
            return true;
        }
        cout << "Lagu tidak ditemukan!" << endl;
        return false;
    }

    // Fungsi untuk menyimpan data lagu ke file
    void saveMusic(string filename)
    {
        ofstream file(filename);
        if (!file.is_open())
        {
            cout << "Error: Tidak bisa menyimpan file!" << endl;
            return;
        }

        for (int i = 0; i < totalSongs; i++)
        {
            file << allSongsArray[i].title << "|"
                 << allSongsArray[i].artist << "|"
                 << allSongsArray[i].duration << "|"
                 << allSongsArray[i].filePath << endl;
        }
        file.close();
        cout << "Data lagu berhasil disimpan!" << endl;
    }
};

#endif