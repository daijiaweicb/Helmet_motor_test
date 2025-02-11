#include <iostream>
#include <gpiod.h>
#include <unistd.h>  // for sleep()

using namespace std;

class Gpio
{
    private:
    gpiod_chip *chipGPIO = nullptr;
    gpiod_line *pinGPIO = nullptr;

    public:
    bool start(int pinNo, int chipNo);
    void setValue(int value);
    void cleanup();
};

bool Gpio::start(int pinNo, int chipNo)
{
    chipGPIO = gpiod_chip_open_by_number(chipNo);
    if (!chipGPIO) {
        cerr << "Failed to open GPIO chip" << endl;
        return false;
    }

    pinGPIO = gpiod_chip_get_line(chipGPIO, pinNo);
    if (!pinGPIO) {
        cerr << "Failed to get GPIO line" << endl;
        gpiod_chip_close(chipGPIO);
        return false;
    }

    // 申请 GPIO 控制权，并设为输出模式
    if (gpiod_line_request_output(pinGPIO, "gpio_test", 0) < 0) {
        cerr << "Failed to set GPIO as output" << endl;
        gpiod_chip_close(chipGPIO);
        return false;
    }

    return true;
}

void Gpio::setValue(int value)
{
    if (pinGPIO) {
        gpiod_line_set_value(pinGPIO, value);
        cout << "GPIO set to: " << value << endl;
    } else {
        cerr << "GPIO not initialized!" << endl;
    }
}

void Gpio::cleanup()
{
    if (pinGPIO) {
        gpiod_line_release(pinGPIO);
    }
    if (chipGPIO) {
        gpiod_chip_close(chipGPIO);
    }
}

int main()
{
    Gpio io1;
    if (io1.start(17, 0)) {
        io1.setValue(1);  // 设置 GPIO 高电平
        io1.cleanup();    // 清理资源
    }
    return 0;
}
