#ifndef KTP_H
#define KTP_H

#include "data.h"
#include "filehandler.h"
#include <fstream>

class KTPManager
{
private:
    struct Node
    {
        KTP *ktp;
        Node *left, *right;
        int height;
        Node(KTP *k) : ktp(k), left(nullptr), right(nullptr), height(1) {}
    };
    Node *root;

    // --- FUNGSI HELPER MANUAL ---
    int maxVal(int a, int b)
    {
        return (a > b) ? a : b;
    }

    int getHeight(Node *N) { return N ? N->height : 0; }

    // Logic Balance Normal (Kiri - Kanan)
    int getBalance(Node *N)
    {
        if (!N)
            return 0;
        return getHeight(N->left) - getHeight(N->right);
    }

    Node *rightRotate(Node *y)
    {
        Node *x = y->left;
        Node *T2 = x->right;
        x->right = y;
        y->left = T2;
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
        x->height = maxVal(getHeight(x->left), getHeight(x->right)) + 1;
        y->height = maxVal(getHeight(y->left), getHeight(y->right)) + 1;
        return y;
    }

    Node *insert(Node *node, KTP *ktp)
    {
        if (!node)
            return new Node(ktp);

        // Sort berdasarkan NIK
        if (ktp->NIK < node->ktp->NIK)
            node->left = insert(node->left, ktp);
        else if (ktp->NIK > node->ktp->NIK)
            node->right = insert(node->right, ktp);
        else
            return node;

        node->height = 1 + maxVal(getHeight(node->left), getHeight(node->right));
        int balance = getBalance(node);

        // Rotasi Normal (LL, RR, LR, RL)
        if (balance > 1 && ktp->NIK < node->left->ktp->NIK)
            return rightRotate(node);
        if (balance < -1 && ktp->NIK > node->right->ktp->NIK)
            return leftRotate(node);
        if (balance > 1 && ktp->NIK > node->left->ktp->NIK)
        {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }
        if (balance < -1 && ktp->NIK < node->right->ktp->NIK)
        {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }
        return node;
    }

    Node *search(Node *node, string nik)
    {
        if (!node || node->ktp->NIK == nik)
            return node;
        if (nik < node->ktp->NIK)
            return search(node->left, nik);
        return search(node->right, nik);
    }

    void saveRec(Node *node, ofstream &file)
    {
        if (node)
        {
            file << node->ktp->NIK << "|" << node->ktp->nama << "|" << node->ktp->alamat << "|" << node->ktp->tanggalLahir << endl;
            saveRec(node->left, file);
            saveRec(node->right, file);
        }
    }

public:
    KTPManager() : root(nullptr) {}

    void registerUser()
    {
        KTP *k = new KTP;
        cout << "NIK: ";
        cin >> k->NIK;
        cin.ignore();
        cout << "Nama: ";
        getline(cin, k->nama);
        cout << "Alamat: ";
        getline(cin, k->alamat);
        cout << "Tgl Lahir: ";
        getline(cin, k->tanggalLahir);
        root = insert(root, k);
        saveToFile();
        cout << "Registrasi Berhasil!" << endl;
    }

    bool login()
    {
        string nik;
        cout << "Login NIK: ";
        cin >> nik;
        Node *res = search(root, nik);
        if (res)
        {
            KTPSekarang = res->ktp; // Set Session Global
            cout << "Login Berhasil! Halo " << KTPSekarang->nama << endl;
            return true;
        }
        cout << "NIK Tidak ditemukan!" << endl;
        return false;
    }

    void saveToFile()
    {
        ofstream file("users.txt");
        saveRec(root, file);
        file.close();
    }

    void loadUsers()
    {
        ifstream file("users.txt");
        if (!file.is_open())
            return;

        string line;
        string parts[10]; // Array manual
        int c;

        while (getline(file, line))
        {
            FileHandler::split(line, '|', parts, c);
            if (c == 4)
            {
                // Buat struct KTP baru
                KTP *k = new KTP;
                k->NIK = parts[0];
                k->nama = parts[1];
                k->alamat = parts[2];
                k->tanggalLahir = parts[3];

                root = insert(root, k);
            }
        }
        file.close();
    }
};

#endif