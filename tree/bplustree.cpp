#include <iostream>
#include <vector>
#include <tuple>
using namespace std;

const int T = 3;

struct Schedule {
    string nid, kmk, id_kelas;

    // Operator pembanding untuk < (kurang dari), > (lebih besar dari), dan >= (lebih besar atau sama dengan)
    bool operator<(const Schedule& other) const {
        return tie(nid, kmk, id_kelas) < tie(other.nid, other.kmk, other.id_kelas);
    }

    bool operator>(const Schedule& other) const {
        return tie(nid, kmk, id_kelas) > tie(other.nid, other.kmk, other.id_kelas);
    }

    bool operator==(const Schedule& other) const {
        return tie(nid, kmk, id_kelas) == tie(other.nid, other.kmk, other.id_kelas);
    }

    bool operator>=(const Schedule& other) const {
        return tie(nid, kmk, id_kelas) >= tie(other.nid, other.kmk, other.id_kelas);
    }

    void print() const {
        cout << "Dosen: " << nid << ", Matkul: " << kmk << ", Kelas: " << id_kelas << endl;
    }
};


class BPlusNode {
public:
    bool leaf;
    vector<Schedule> keys;
    vector<BPlusNode*> children;
    BPlusNode* next; // Only for leaf

    BPlusNode(bool leaf) : leaf(leaf), next(nullptr) {}
};

class BPlusTree {
public:
    BPlusNode* root = nullptr;

    void insert(Schedule k);
    Schedule* search(Schedule k);
    void traverse(); // print all leaves
private:
    void insertNonFull(BPlusNode* node, Schedule k);
    void splitChild(BPlusNode* parent, int i);
};

void BPlusTree::traverse() {
    BPlusNode* cur = root;
    while (cur && !cur->leaf) cur = cur->children[0];
    while (cur) {
        for (auto& k : cur->keys) k.print();
        cur = cur->next;
    }
}

Schedule* BPlusTree::search(Schedule k) {
    BPlusNode* cur = root;
    while (cur && !cur->leaf) {
        size_t i = 0;
        while (i < cur->keys.size() && k >= cur->keys[i]) i++;
        cur = cur->children[i];
    }
    if (!cur) return nullptr;
    for (auto& key : cur->keys)
        if (key == k) return &key;
    return nullptr;
}

void BPlusTree::insert(Schedule k) {
    if (!root) {
        root = new BPlusNode(true);
        root->keys.push_back(k);
        return;
    }

    if (root->keys.size() == 2 * T - 1) {
        BPlusNode* newRoot = new BPlusNode(false);
        newRoot->children.push_back(root);
        splitChild(newRoot, 0);
        root = newRoot;
    }
    insertNonFull(root, k);
}

void BPlusTree::insertNonFull(BPlusNode* node, Schedule k) {
    int i = node->keys.size() - 1;

    if (node->leaf) {
        node->keys.push_back(k);
        while (i >= 0 && k < node->keys[i]) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = k;
    } else {
        while (i >= 0 && k < node->keys[i]) i--;
        i++;
        if (node->children[i]->keys.size() == 2 * T - 1)
            splitChild(node, i);
        insertNonFull(node->children[i], k);
    }
}

void BPlusTree::splitChild(BPlusNode* parent, int i) {
    BPlusNode* y = parent->children[i];
    BPlusNode* z = new BPlusNode(y->leaf);

    z->keys.assign(y->keys.begin() + T, y->keys.end());
    y->keys.resize(T - 1);

    if (!y->leaf) {
        z->children.assign(y->children.begin() + T, y->children.end());
        y->children.resize(T);
    } else {
        z->next = y->next;
        y->next = z;
    }

    parent->keys.insert(parent->keys.begin() + i, z->keys[0]);
    parent->children.insert(parent->children.begin() + i + 1, z);
}

int main() {
    /// Untuk B+ Tree
    BPlusTree tree;
    tree.insert({"D001", "MK001", "KLS01"});
    tree.insert({"D002", "MK002", "KLS02"});
    tree.insert({"D003", "MK003", "KLS03"});
    tree.traverse();
    Schedule* found = tree.search({"D002", "MK002", "KLS02"});
    if (found) cout << "Found: ", found->print();

    

    return 0;
}
