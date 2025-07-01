#include <iostream>
#include <vector>
using namespace std;

const int T = 3; 


struct Schedule {
    string nid, kmk, id_kelas;
    
    bool operator<(const Schedule& other) const {
        return tie(nid, kmk, id_kelas) < tie(other.nid, other.kmk, other.id_kelas);
    }

    bool operator>(const Schedule& other) const {
        return tie(nid, kmk, id_kelas) > tie(other.nid, other.kmk, other.id_kelas);
    }

    bool operator==(const Schedule& other) const {
        return tie(nid, kmk, id_kelas) == tie(other.nid, other.kmk, other.id_kelas);
    }

    void print() const {
        cout << "Dosen: " << nid << ", Matkul: " << kmk << ", Kelas: " << id_kelas << endl;
    }
};

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

    Schedule* search(Schedule k) {
        size_t i = 0;
        while (i < keys.size() && k > keys[i]) i++;
        if (i < keys.size() && keys[i] == k) return &keys[i];
        if (leaf) return nullptr;
        return children[i]->search(k);
    }

    void insertNonFull(Schedule k);
    void splitChild(int i, BTreeNode* y);
};

class BTree {
public:
    BTreeNode* root = nullptr;

    void insert(Schedule k);
    void traverse() {
        if (root) root->traverse();
    }

    Schedule* search(Schedule k) {
        return root ? root->search(k) : nullptr;
    }
};

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


int main() {
    // Untuk BTree biasa
    BTree tree;
    tree.insert({"D001", "MK001", "KLS01"});
    tree.insert({"D002", "MK002", "KLS02"});
    tree.insert({"D001", "MK003", "KLS01"});
    tree.traverse();

    Schedule* found = tree.search({"D002", "MK002", "KLS02"});
    if (found) cout << "Found: ", found->print();

    return 0;
}
