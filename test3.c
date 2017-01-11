#include <wiringPi.h>
#include <wiringShift.h>
#include <stdio.h>

#define ClockPin 0	// GPIO17 on PI 3
#define DataPin 1	// GPIO18 on PI 3
#define LightCount 50

void clockAll() {
	int bits = 24 * 50;
	while (--bits >= 0) {
		digitalWrite(ClockPin, HIGH);
		delayMicroseconds(1);
		digitalWrite(ClockPin, LOW);
		delayMicroseconds(1);
	}
}

void clockDelay() {
	int count = 0;
	while (--count >= 0);
}

// The ws2801 can handle 25 Mhz clock, but I can't accurately time
// it here.  Maybe a kernel module can do it at some point in the 
// futue or a microcontroller.
void shiftRgb(int color) {
	int count = 24;
	while (--count >= 0) {
		digitalWrite(DataPin, (color & 0x800000) ? HIGH : LOW);
		clockDelay();
		digitalWrite(ClockPin, HIGH);
		delayMicroseconds(1);
		digitalWrite(ClockPin, LOW);
		delayMicroseconds(1);
		color <<= 1;	// shift it upward 1 bit
	}
	
}

void setAll(int rgb, int count) {
	while (--count >= 0) {
		shiftRgb(rgb);
	}
}

int main(void) {
	if(wiringPiSetup() == -1){ //when initialize wiring failed,print messageto screen
		printf("setup wiringPi failed !");
		return 1; 
	}

	pinMode(ClockPin, OUTPUT);
	pinMode(DataPin, OUTPUT);

	setAll(0, LightCount);

	int i;
	for (i = 0; i < 256; ++i) {
		setAll(i << 16, LightCount);
		delay(15);	// about 60fps
	}

	for (i = 254; i >= 0; --i) {
		setAll(i << 16, LightCount);
		delay(15);	// about 60fps
	}


	for (i = 0; i < 256; ++i) {
		setAll(i << 8, LightCount);
		delay(15);	// about 60fps
	}

	for (i = 254; i >= 0; --i) {
		setAll(i << 8, LightCount);
		delay(15);	// about 60fps
	}

	for (i = 0; i < 256; ++i) {
		setAll(i, LightCount);
		delay(15);	// about 60fps
	}

	for (i = 254; i >= 0; --i) {
		setAll(i, LightCount);
		delay(15);	// about 60fps
	}


	setAll(0, LightCount);

	printf("Done!\n");

	return 0;
}


