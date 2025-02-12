#include "Test.h"


void Test_motor()
{
    StepperMotor motor;
    if (motor.start(0, 17, 27, 22))
    {
        std::cout << "Moving Forward" << std::endl;
        motor.forward(1024);
        sleep(2);

        std::cout << "Moving Backward" << std::endl;
        motor.backward(1024);
        sleep(2);

        motor.cleanup();
    }
}