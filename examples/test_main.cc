#include <iostream>
using namespace std;

extern "C" {
    double avg(double, double);
    double distance(double, double, double, double);
    double factorial(double);
}

int main() {
    cout << "average of 13.0 and 14.0: " << avg(13.0, 14.0) << endl;
    cout << "distance between (1, 2) and (4,6): " << distance(1, 2, 4, 6) << endl;
    cout << "factorial of 5!: " << factorial(5) << endl;
}