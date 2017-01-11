#include <wiringPi.h>
#include <wiringShift.h>
#include <stdio.h>
#include <stdlib.h>

#define ClockPin 0	// GPIO17 on PI 3
#define DataPin 1	// GPIO18 on PI 3
#define LightCount 50

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

void setEach(int* rgb, int count) {
	int total = count;
	while (count >= 0) {
		shiftRgb(rgb[total - count]);
		--count;
	}
}

int randomColor(int max) {
	max &= 0xff;
	int red = random() % max;
	int green = random() % max;
	int blue = random() % max;
	return red << 16 | green << 8 | blue;
}

void fillRandomColors(int* rgb, int count) {
	while (--count >= 0) {
		rgb[count] = randomColor(16);
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

	int* colors = (int*)malloc(sizeof(int) * LightCount);
	int count = 100;
	while(--count >= 0) {	
		fillRandomColors(colors, LightCount);

		setEach(colors, LightCount);

		delay(100);
	}

	setAll(0, LightCount);

	free(colors);
	colors = 0;

	printf("Done!\n");

	return 0;
}


