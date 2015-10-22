#include "mbed-drivers/mbed.h"
#include "htu21d/htu21d.h"

static void blinky(void) {
	static Serial pc(PTC17,PTC16);
    static DigitalOut led(LED1);
    led = !led;

	htu21d htu(PTE25, PTE24); //Temp humid sensor || SDA, SCL
	float current_temperature_value = htu.getTemp();
	float current_humidity_value = htu.getHum();
	int rounded_temp = (int)current_temperature_value;
	int rounded_hum = (int)current_humidity_value;
	pc.printf("Temperature: %i\r\n", rounded_temp);
	pc.printf("Humidity: %i\r\n", rounded_hum);
}

void app_start(int, char**) {
    minar::Scheduler::postCallback(blinky).period(minar::milliseconds(2000));
}

