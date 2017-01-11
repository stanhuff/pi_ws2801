#include <wiringPi.h>
#include <wiringShift.h>
#include <stdio.h>

#define ClockPin 0	// GPIO17 on PI 3
#define DataPin 1	// GPIO18 on PI 3

void clockAll() {
	int bits = 24 * 50;
	while (--bits >= 0) {
		digitalWrite(ClockPin, HIGH);
		delayMicroseconds(1);
		digitalWrite(ClockPin, LOW);
		delayMicroseconds(1);
	}
}

int main(void) {
	if(wiringPiSetup() == -1){ //when initialize wiring failed,print messageto screen
		printf("setup wiringPi failed !");
		return 1; 
	}

	pinMode(ClockPin, OUTPUT);
	pinMode(DataPin, OUTPUT);

	digitalWrite(ClockPin, LOW);
	digitalWrite(DataPin, LOW);

	delay(1);

	digitalWrite(DataPin, LOW);

	// show white color on the first LED
	clockAll();

	delay(1000);

	digitalWrite(DataPin, HIGH);

	clockAll();

	delay(10000);

	digitalWrite(DataPin, LOW);

	clockAll();

	digitalWrite(DataPin, LOW);

	printf("Done!\n");

	return 0;
}


