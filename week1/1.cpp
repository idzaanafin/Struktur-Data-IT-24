#include <iostream>
using namespace std;

class MHS
{
	public:
	string MHSname;
	void  printname()
	{
		cout << "MHS name is:" << MHSname;
	}
};

int main(){
	MHS mhs1;
	mhs1.MHSname = "yudi gaming";
	mhs1.printname();
	return 0;
}
