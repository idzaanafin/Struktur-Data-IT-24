#include <iostream>
#include <vector>
#include <tuple>
#include <string>
using namespace std;

const int T = 3; 




struct Schedule {
    string nid;       
    string kmk;       
    string kelas;     
    string hari;      
    string jam;       
    string ruang;     

    
    bool operator<(const Schedule& other) const {
        return tie(hari, jam, ruang) < tie(other.hari, other.jam, other.ruang);
    }

    bool operator>(const Schedule& other) const {
        return tie(hari, jam, ruang) > tie(other.hari, other.jam, other.ruang);
    }

    bool operator==(const Schedule& other) const {
        return tie(hari, jam, ruang) == tie(other.hari, other.jam, other.ruang);
    }

    void print() const {
        cout << "Dosen: " << nid << ", MK: " << kmk
             << ", Kelas: " << kelas << ", Hari: " << hari
             << ", Jam: " << jam << ", Ruang: " << ruang << endl;
    }
};

// -----------------------------
// B-Tree Node
// -----------------------------
class BTreeNode {
public:
    bool leaf;
    vector<Schedule> keys;
    vector<BTreeNode*> children;

    BTreeNode(bool leaf) : leaf(leaf) {}

    void traverse() {
        for (size_t i = 0; i < keys.size(); i++) {
            if (!leaf) children[i]->traverse();
            keys[i].print();
        }
        if (!leaf) children[keys.size()]->traverse();
    }

    Schedule* search(const Schedule& k) {
        size_t i = 0;
        while (i < keys.size() && k > keys[i]) i++;
        if (i < keys.size() && keys[i] == k) return &keys[i];
        if (leaf) return nullptr;
        return children[i]->search(k);
    }

    void insertNonFull(Schedule k);
    void splitChild(int i, BTreeNode* y);
};

// -----------------------------
// B-Tree Class
// -----------------------------
class BTree {
public:
    BTreeNode* root = nullptr;

    void insert(Schedule k);
    Schedule* search(const Schedule& k) {
        return root ? root->search(k) : nullptr;
    }

    void traverse() {
        if (root) root->traverse();
    }
};

// -----------------------------
// BTree::insert
// -----------------------------
void BTree::insert(Schedule k) {
    if (!root) {
        root = new BTreeNode(true);
        root->keys.push_back(k);
    } else {
        if (root->keys.size() == 2 * T - 1) {
            BTreeNode* s = new BTreeNode(false);
            s->children.push_back(root);
            s->splitChild(0, root);
            int i = (k < s->keys[0]) ? 0 : 1;
            s->children[i]->insertNonFull(k);
            root = s;
        } else {
            root->insertNonFull(k);
        }
    }
}

// -----------------------------
// BTreeNode::insertNonFull
// -----------------------------
void BTreeNode::insertNonFull(Schedule k) {
    int i = keys.size() - 1;
    if (leaf) {
        keys.push_back(k);
        while (i >= 0 && k < keys[i]) {
            keys[i + 1] = keys[i];
            i--;
        }
        keys[i + 1] = k;
    } else {
        while (i >= 0 && k < keys[i]) i--;
        i++;
        if (children[i]->keys.size() == 2 * T - 1) {
            splitChild(i, children[i]);
            if (k > keys[i]) i++;
        }
        children[i]->insertNonFull(k);
    }
}

// -----------------------------
// BTreeNode::splitChild
// -----------------------------
void BTreeNode::splitChild(int i, BTreeNode* y) {
    BTreeNode* z = new BTreeNode(y->leaf);
    z->keys.assign(y->keys.begin() + T, y->keys.end());
    y->keys.resize(T - 1);

    if (!y->leaf) {
        z->children.assign(y->children.begin() + T, y->children.end());
        y->children.resize(T);
    }

    children.insert(children.begin() + i + 1, z);
    keys.insert(keys.begin() + i, y->keys[T - 1]);
}

// -----------------------------
// MAIN
// -----------------------------
int main() {
    BTree jadwalKuliah;

    vector<Schedule> data = {
        {"D001", "MK001", "A", "Senin", "08:00", "R101"},
        {"D002", "MK002", "B", "Selasa", "09:00", "R102"},
        {"D003", "MK003", "A", "Rabu", "10:00", "R101"},
        {"D004", "MK004", "C", "Kamis", "11:00", "R103"},
        {"D005", "MK005", "A", "Jumat", "13:00", "R104"},
        {"D001", "MK001", "A", "Senin", "08:00", "R101"}, // Jadwal bentrok
        {"D006", "MK006", "B", "Selasa", "09:00", "R102"}, // Jadwal bentrok
    };

    cout << "=== PENJADWALAN KULIAH PENGGANTI ===\n\n";

    for (const auto& sch : data) {
        if (jadwalKuliah.search(sch)) {
            cout << "Jadwal bentrok: ";
            sch.print();
        } else {
            jadwalKuliah.insert(sch);
            cout << "Jadwal berhasil ditambahkan: ";
            sch.print();
        }
    }

    cout << "\nDaftar Jadwal Kuliah:\n";
    jadwalKuliah.traverse();

    return 0;
}
