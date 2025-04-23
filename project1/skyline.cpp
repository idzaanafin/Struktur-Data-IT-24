#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <chrono>

using namespace std;
using namespace std::chrono;


struct Product {
    string name;
    int attr[2];
};


bool mendominasi(const Product& a, const Product& b) {
    return (a.attr[0] <= b.attr[0] && a.attr[1] >= b.attr[1]) &&
           (a.attr[0] < b.attr[0] || a.attr[1] > b.attr[1]);
}


void skylineQuery(const unordered_map<string, Product>& data, unordered_map<string, Product>& result) {
    result.clear(); 

    for (const auto& pair : data) {
        const Product& produk = pair.second;
        bool didominasi = false;

        for (auto it = result.begin(); it != result.end();) {
            if (mendominasi(it->second, produk)) {
                didominasi = true;
                break;
            }
            if (mendominasi(produk, it->second)) {
                it = result.erase(it); 
            } else {
                ++it;
            }
        }

        if (!didominasi) {
            result[produk.name] = produk; 
        }
    }
}

void readCSV(const string& filename, unordered_map<string, Product>& data) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Failed to open file.\n";
        return;
    }

    string line;
    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        string id, name, attr1, attr2;
        getline(ss, id, ',');
        getline(ss, name, ',');
        getline(ss, attr1, ',');
        getline(ss, attr2, ',');

        data[name] = {name, {stoi(attr1), stoi(attr2)}};
    }

    file.close();
}

void cetakSkyline(const unordered_map<string, Product>& result) {
    for (const auto& [key,value] : result) {
        cout << key << " - attr1: " << value.attr[0] << ", attr2: " << value.attr[1] << endl;
    }
}


int main() {
    string filename = "ind_1000_2_product.csv";
    unordered_map<string, Product> products;
    unordered_map<string, Product> skyline_result;

    readCSV(filename, products);
    
    auto start = high_resolution_clock::now();

    skylineQuery(products, skyline_result);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Waktu komputasi: " << duration.count() << " ms" << endl;

    cout << "Hasil Skyline:\n";
    cetakSkyline(skyline_result);

    cout << "Jumlah produk dalam skyline: " << skyline_result.size() << endl;
    cout << "Jumlah produk dalam data: " << products.size() << endl;


    return 0;
}
