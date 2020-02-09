#include <iostream>
using namespace std;

extern "C" {
    double avg(double, double);
}

int main() {
    cout << "average of 13.0 and 14.0: " << avg(13.0, 14.0) << endl;
}