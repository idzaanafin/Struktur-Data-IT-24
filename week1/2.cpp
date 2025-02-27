#include <iostream>
using namespace std;

class MHS {
private:
    string MHSname;

public:
    MHS(string name) {
        MHSname = name;
    }

    void setName(string name) {
        MHSname = name;
    }

    string getNama() const {
        return MHSname;
    }

    void printName() const {
        cout << "MHS name is: " << MHSname << endl;
    }
};

int main() {
    MHS mhs1("yudi gaming");
    mhs1.printName();
    MHS mhs2("pujo ganteng")

    mhs1.printName();

    cout << "Accessed name: " << mhs1.getName() << endl;

    return 0;
}
