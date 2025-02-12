#include "motor.h"

using namespace std;

bool StepperMotor::start(int chipNo, int pin1, int pin2, int pin3, int pin4)
{
    chipGPIO = gpiod_chip_open_by_number(chipNo);
    if (!chipGPIO)
    {
        cout << "Failed to open GPIO chip" << endl;
        return false;
    }

    gpio_pins[0] = pin1;
    gpio_pins[1] = pin2;
    gpio_pins[2] = pin3;
    gpio_pins[3] = pin4;

    for (int i = 0; i < 4; i++)
    {
        pins[i] = gpiod_chip_get_line(chipGPIO, gpio_pins[i]);
        if (!pins[i])
        {
            cout << "Failed to get GPIO line " << gpio_pins[i] << endl;
            cleanup();
            return false;
        }

        if (gpiod_line_request_output(pins[i], "stepper_motor", 0) < 0)
        {
            cout << "Failed to set GPIO " << gpio_pins[i] << " as output" << endl;
            cleanup();
            return false;
        }
    }

    return true;
}

void StepperMotor::step(int stepPattern[3])
{
    for (int i = 0; i < 4; i++)
    {
        gpiod_line_set_value(pins[i], stepPattern[i]);
    }
    usleep(step_delay);
}

void StepperMotor::forward(int steps)
{
    int stepSequence[9][4] = {
        {1, 0, 0, 0},  // IN1
        {1, 1, 0, 0},  // IN1 + IN2
        {0, 1, 0, 1},  // IN2
        {0, 1, 1, 1},  // IN2 + IN3
        {0, 0, 1, 1},  // IN3
        {1, 0, 1, 1},   // IN3 + IN1
        {0, 0, 1, 1},
        {0, 1, 1, 1},
        {0, 1, 1, 0}
    };

    for (int i = 0; i < steps; i++)
    {
        step(stepSequence[i % 9]);
    }
}

void StepperMotor::backward(int steps)
{
    int stepSequence[9][4] = {
        {0, 1, 1, 0},
        {0, 1, 1, 1},
        {0, 0, 1, 1},
        {1, 0, 1, 1},
        {0, 0, 1, 1},
        {0, 1, 1, 1},
        {0, 1, 0, 1},
        {1, 1, 0, 0},
        {1, 0, 0, 0},
    };

    for (int i = 0; i < steps; i++)
    {
        step(stepSequence[i % 9]);
    }
}

void StepperMotor::cleanup()
{
    for (int i = 0; i < 4; i++)
    {
        if (pins[i])
        {
            gpiod_line_set_value(pins[i], 0);
            gpiod_line_release(pins[i]);
        }
    }
    if (chipGPIO)
    {
        gpiod_chip_close(chipGPIO);
    }
}