#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <unordered_map>
#include <map>
#include <list>
#include <stack>
#include <queue>

using namespace std;
using namespace chrono;

struct Product {
    string id;
    string name;
    int attr[2];
};

bool dominates(const Product& a, const Product& b) {
    return (a.attr[0] <= b.attr[0] && a.attr[1] >= b.attr[1]) &&
           (a.attr[0] < b.attr[0] || a.attr[1] > b.attr[1]);
}

void loadCSV(const string& filename, Product arr[], int& count) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file." << endl;
        return;
    }
    string line;
    getline(file, line); // skip header
    count = 0;
    while (getline(file, line)) {
        stringstream ss(line);
        string id, name, attr1, attr2;
        getline(ss, id, ',');
        getline(ss, name, ',');
        getline(ss, attr1, ',');
        getline(ss, attr2, ',');
        arr[count++] = {id, name, {stoi(attr1), stoi(attr2)}};
    }
    file.close();
}

int skylineArray(Product data[], int count) {
    bool dominated;
    int skylineCount = 0;
    for (int i = 0; i < count; ++i) {
        dominated = false;
        for (int j = 0; j < count; ++j) {
            if (i != j && dominates(data[j], data[i])) {
                dominated = true;
                break;
            }
        }
        if (!dominated) skylineCount++;
    }
    return skylineCount;
}

int skylineMap(map<string, Product>& data) {
    map<string, Product> skyline;
    for (const auto& [id, p] : data) {
        bool dominated = false;
        for (auto it = skyline.begin(); it != skyline.end();) {
            if (dominates(it->second, p)) {
                dominated = true;
                break;
            }
            if (dominates(p, it->second)) {
                it = skyline.erase(it);
            } else {
                ++it;
            }
        }
        if (!dominated) skyline[id] = p;
    }
    return skyline.size();
}

int skylineUnorderedMap(unordered_map<string, Product>& data) {
    unordered_map<string, Product> skyline;
    for (const auto& [id, p] : data) {
        bool dominated = false;
        for (auto it = skyline.begin(); it != skyline.end();) {
            if (dominates(it->second, p)) {
                dominated = true;
                break;
            }
            if (dominates(p, it->second)) {
                it = skyline.erase(it);
            } else {
                ++it;
            }
        }
        if (!dominated) skyline[id] = p;
    }
    return skyline.size();
}

int skylineLinkedList(list<Product>& data) {
    list<Product> skyline;
    for (const auto& p : data) {
        bool dominated = false;
        for (auto it = skyline.begin(); it != skyline.end();) {
            if (dominates(*it, p)) {
                dominated = true;
                break;
            }
            if (dominates(p, *it)) {
                it = skyline.erase(it);
            } else {
                ++it;
            }
        }
        if (!dominated) skyline.push_back(p);
    }
    return skyline.size();
}

int skylineStack(stack<Product>& data) {
    stack<Product> skyline;
    while (!data.empty()) {
        Product p = data.top();
        bool dominated = false;
        for (auto tempStack = skyline; !tempStack.empty();) {
            if (dominates(tempStack.top(), p)) {
                dominated = true;
                break;
            }
            if (dominates(p, tempStack.top())) {
                tempStack.pop();
            } else {
                tempStack.pop();
            }
        }
        if (!dominated) skyline.push(p);
        data.pop();
    }
    return skyline.size();
}

int skylineQueue(queue<Product>& data) {
    queue<Product> skyline;
    while (!data.empty()) {
        Product p = data.front();
        bool dominated = false;
        for (auto tempQueue = skyline; !tempQueue.empty();) {
            if (dominates(tempQueue.front(), p)) {
                dominated = true;
                break;
            }
            if (dominates(p, tempQueue.front())) {
                tempQueue.pop();
            } else {
                tempQueue.pop();
            }
        }
        if (!dominated) skyline.push(p);
        data.pop();
    }
    return skyline.size();
}

int main() {
    const string filename = "ind_1000_2_product.csv";

    // Array
    Product dataArr[1000];
    int count = 0;
    loadCSV(filename, dataArr, count);
    auto start = high_resolution_clock::now();
    int resArr = skylineArray(dataArr, count);
    auto end = high_resolution_clock::now();
    cout << "Skyline with array: " << resArr << " | Time: "
         << duration_cast<microseconds>(end - start).count() << " us" << endl;

    // Map
    map<string, Product> mapData;
    for (int i = 0; i < count; ++i) mapData[dataArr[i].id] = dataArr[i];
    start = high_resolution_clock::now();
    int resMap = skylineMap(mapData);
    end = high_resolution_clock::now();
    cout << "Skyline with map: " << resMap << " | Time: "
         << duration_cast<microseconds>(end - start).count() << " us" << endl;

    // Unordered Map
    unordered_map<string, Product> umapData;
    for (int i = 0; i < count; ++i) umapData[dataArr[i].id] = dataArr[i];
    start = high_resolution_clock::now();
    int resUMap = skylineUnorderedMap(umapData);
    end = high_resolution_clock::now();
    cout << "Skyline with unordered_map: " << resUMap << " | Time: "
         << duration_cast<microseconds>(end - start).count() << " us" << endl;

    // Linked List
    list<Product> listData(dataArr, dataArr + count);
    start = high_resolution_clock::now();
    int resList = skylineLinkedList(listData);
    end = high_resolution_clock::now();
    cout << "Skyline with linked list: " << resList << " | Time: "
         << duration_cast<microseconds>(end - start).count() << " us" << endl;

    // Stack
    stack<Product> stackData;
    for (int i = 0; i < count; ++i) stackData.push(dataArr[i]);
    start = high_resolution_clock::now();
    int resStack = skylineStack(stackData);
    end = high_resolution_clock::now();
    cout << "Skyline with stack: " << resStack << " | Time: "
         << duration_cast<microseconds>(end - start).count() << " us" << endl;

    // Queue
    queue<Product> queueData;
    for (int i = 0; i < count; ++i) queueData.push(dataArr[i]);
    start = high_resolution_clock::now();
    int resQueue = skylineQueue(queueData);
    end = high_resolution_clock::now();
    cout << "Skyline with queue: " << resQueue << " | Time: "
         << duration_cast<microseconds>(end - start).count() << " us" << endl;

    return 0;
}
