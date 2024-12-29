#include <Arduino.h>
class Switch{
	private:
		boolean wasLow = false;
		int pin = 0;
		int mem = 0;

	public:
		Switch(int pin);
		boolean toggle();
		boolean isOn();
		boolean isOff();
};
