#include <iostream>
#include "Image.h"

using namespace std;

int main()
{
    cout << "Hello There" << endl;
    Image test("test.jpg");

    double ker[] = {1/16.0, 2/16.0, 1/16.0,
                    2/16.0, 4/16.0, 2/16.0,
                    1/16.0, 2/16.0, 1/16.0};
    test.conv_simple_to_zero(0, 3, 3, ker, 1, 1);
    test.conv_simple_to_zero(1, 3, 3, ker, 1, 1);
    test.conv_simple_to_zero(2, 3, 3, ker, 1, 1);
    test.write("bluerred.png");

    
    return 0;
}
