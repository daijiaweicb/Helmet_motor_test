#include <iostream>
#include <gpiod.h>
#include <unistd.h>  // for usleep()

using namespace std;

class StepperMotor
{
private:
    gpiod_chip *chipGPIO = nullptr;
    gpiod_line *pins[3] = {nullptr};  // 3 个 GPIO 控制线
    int gpio_pins[3]; // 存储 GPIO 编号
    int step_delay = 2000; // 步进延迟（微秒）

public:
    bool start(int chipNo, int pin1, int pin2, int pin3);
    void forward(int steps);
    void backward(int steps);
    void cleanup();
    void step(int stepPattern[3]);
};

bool StepperMotor::start(int chipNo, int pin1, int pin2, int pin3)
{
    chipGPIO = gpiod_chip_open_by_number(chipNo);
    if (!chipGPIO)
    {
        cerr << "Failed to open GPIO chip" << endl;
        return false;
    }

    gpio_pins[0] = pin1;
    gpio_pins[1] = pin2;
    gpio_pins[2] = pin3;

    for (int i = 0; i < 3; i++)
    {
        pins[i] = gpiod_chip_get_line(chipGPIO, gpio_pins[i]);
        if (!pins[i])
        {
            cerr << "Failed to get GPIO line " << gpio_pins[i] << endl;
            cleanup();
            return false;
        }

        if (gpiod_line_request_output(pins[i], "stepper_motor", 0) < 0)
        {
            cerr << "Failed to set GPIO " << gpio_pins[i] << " as output" << endl;
            cleanup();
            return false;
        }
    }

    return true;
}

void StepperMotor::step(int stepPattern[3])
{
    for (int i = 0; i < 3; i++)
    {
        gpiod_line_set_value(pins[i], stepPattern[i]);
    }
    usleep(step_delay);
}

void StepperMotor::forward(int steps)
{
    int stepSequence[6][3] = {
        {1, 0, 0},  // IN1
        {1, 1, 0},  // IN1 + IN2
        {0, 1, 0},  // IN2
        {0, 1, 1},  // IN2 + IN3
        {0, 0, 1},  // IN3
        {1, 0, 1}   // IN3 + IN1
    };

    for (int i = 0; i < steps; i++)
    {
        step(stepSequence[i % 6]);
    }
}

void StepperMotor::backward(int steps)
{
    int stepSequence[6][3] = {
        {1, 0, 1},  // IN3 + IN1
        {0, 0, 1},  // IN3
        {0, 1, 1},  // IN2 + IN3
        {0, 1, 0},  // IN2
        {1, 1, 0},  // IN1 + IN2
        {1, 0, 0}   // IN1
    };

    for (int i = 0; i < steps; i++)
    {
        step(stepSequence[i % 6]);
    }
}

void StepperMotor::cleanup()
{
    for (int i = 0; i < 3; i++)
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

int main()
{
    StepperMotor motor;
    if (motor.start(0, 17, 27, 22)) // 连接 GPIO17, 27, 22
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
