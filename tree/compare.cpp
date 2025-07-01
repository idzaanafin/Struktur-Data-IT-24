#include <iostream>
#include <vector>
#include <tuple>
#include <chrono>
using namespace std;
using namespace std::chrono;

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

    bool operator>=(const Schedule& other) const {
        return *this > other || *this == other; // Explicitly combine `>` and `==`
    }

    void print() const {
        cout << "Dosen: " << nid << ", Matkul: " << kmk << ", Kelas: " << id_kelas << endl;
    }
};

// B+ Tree
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

// B-Tree
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
    vector<Schedule> schedules = {
        {"D001", "MK001", "KLS01"},
        {"D002", "MK002", "KLS02"},
        {"D003", "MK003", "KLS03"},
        {"D004", "MK004", "KLS04"},
        {"D005", "MK005", "KLS05"},
        {"D006", "MK006", "KLS06"},
        {"D007", "MK007", "KLS07"},
        {"D008", "MK008", "KLS08"}
    };

    // Test B+ Tree
    BPlusTree bplusTree;

    auto startBPlus = high_resolution_clock::now();
    for (auto& sched : schedules) {
        bplusTree.insert(sched);
    }
    auto stopBPlus = high_resolution_clock::now();
    auto durationBPlus = duration_cast<microseconds>(stopBPlus - startBPlus);

    cout << "B+ Tree Insert Time: " << durationBPlus.count() << " microseconds" << endl;

    auto startBPlusSearch = high_resolution_clock::now();
    bplusTree.search({"D002", "MK002", "KLS02"});
    auto stopBPlusSearch = high_resolution_clock::now();
    auto durationBPlusSearch = duration_cast<microseconds>(stopBPlusSearch - startBPlusSearch);

    cout << "B+ Tree Search Time: " << durationBPlusSearch.count() << " microseconds" << endl;

    // Test B-Tree
    BTree btree;

    auto startBTree = high_resolution_clock::now();
    for (auto& sched : schedules) {
        btree.insert(sched);
    }
    auto stopBTree = high_resolution_clock::now();
    auto durationBTree = duration_cast<microseconds>(stopBTree - startBTree);

    cout << "B Tree Insert Time: " << durationBTree.count() << " microseconds" << endl;

    auto startBTreeSearch = high_resolution_clock::now();
    btree.search({"D002", "MK002", "KLS02"});
    auto stopBTreeSearch = high_resolution_clock::now();
    auto durationBTreeSearch = duration_cast<microseconds>(stopBTreeSearch - startBTreeSearch);

    cout << "B Tree Search Time: " << durationBTreeSearch.count() << " microseconds" << endl;

    return 0;
}
