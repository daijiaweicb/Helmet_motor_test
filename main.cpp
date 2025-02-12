#include <iostream>
#include <gpiod.h>
#include <unistd.h>  
#include "motor.h"

using namespace std;

int main()
{
    StepperMotor motor;
    if (motor.start(0, 17, 27, 22))
    {
        cout << "Moving Forward" << endl;
        motor.forward(1024);
        sleep(2);

        cout << "Moving Backward" << endl;
        motor.backward(1024);
        sleep(2);

        motor.cleanup();
    }

    return 0;
}
