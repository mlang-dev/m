#include <iostream>
using namespace std;

extern "C" {
    double avg(double, double);
    double add(double, double);
    double distance(double, double, double, double);
}

int main() {
    cout << "average of 13.0 and 14.0: " << avg(13.0, 14.0) << endl;
    cout << "add 13.0 and 14.0: " << add(13.0, 14.0) << endl;
    cout << "distance between (1, 2) and (4,6): " << distance(1, 2, 4, 6) << endl;
}