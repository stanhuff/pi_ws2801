#include <wiringPi.h>
#include <wiringShift.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ClockPin 0	// GPIO17 on PI 3
#define DataPin 1	// GPIO18 on PI 3
#define LightCount 50

#define GET_RED(c) ((c >> 16) & 0xff)
#define GET_GREEN(c) ((c >> 8) & 0xff)
#define GET_BLUE(c) (c & 0xff)

#define MAKE_COLOR(r, g, b) (r << 16 | g << 8 | b)
#define OR_RED(c, r) (c | (r << 16))
#define OR_GREEN(c, g) (c | (g << 8))
#define OR_BLUE(c, b) (c | b)

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

int lerp(float t, int start, int end) {
	int range = end - start;
	return (int)(t * range) + start;
}

int lerpColor(float t, int start, int end) {
	int red = lerp(t, GET_RED(start), GET_RED(end));
	int green = lerp(t, GET_GREEN(start), GET_GREEN(end));
	int blue = lerp(t, GET_BLUE(start), GET_BLUE(end));
	return MAKE_COLOR(red, green, blue);
}

float easeIn(float t) {
	return t * t;
}

// Bias the colors closer to black because using a linear scale,
// the colors very quickly become bright and then increases produce
// very indiscernable differences.
int normalizeColor(int color) {
	int red = easeIn(GET_RED(color) / 255.0f);	
	int green = easeIn(GET_GREEN(color) / 255.0f);	
	int blue = easeIn(GET_BLUE(color) / 255.0f);	
	return MAKE_COLOR(red, green, blue);
}

void fadeEach(float t, int* startColors, int* endColors, int count) {
	int total = count;
	while (count >= 0) {
		int index = total - count;
		int faded = lerpColor(t, startColors[index], endColors[index]);
		shiftRgb(faded);
		--count;
	}
}

int randomQuantized(int max, int quantize) {
	if (quantize < 2)
		quantize = 2;
	return (random() % quantize) / (float)(quantize - 1) * max;
}

int randomQuantizedColor(int max, int quantize) {
	max &= 0xff;
	int red = randomQuantized(max, quantize);
	int green = randomQuantized(max, quantize);
	int blue = randomQuantized(max, quantize);
	return MAKE_COLOR(red, green, blue);
}

int randomColor(int max) {
	max &= 0xff;
	int red = random() % max;
	int green = random() % max;
	int blue = random() % max;
	return OR_RED(OR_GREEN(blue, green), red);
}

void fillRandomColors(int* rgb, int count) {
	while (--count >= 0) {
		rgb[count] = randomColor(255);
	}
}

void fillRandomQuantizedColors(int* rgb, int count, int quantize) {
	while (--count >= 0) {
		rgb[count] = randomQuantizedColor(128, quantize);
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

	int* colors = (int*)malloc(sizeof(int) * LightCount * 2);

	int* from = colors;
	int* to = &colors[LightCount];

	memset(from, 0, LightCount * sizeof(int));

	int count = 100;
	while(--count >= 0) {	
		fillRandomQuantizedColors(to, LightCount, 2);

		int step;
		for (step = 0; step < 100; ++step) {
			float t = step / 100.0f;
			fadeEach(t, from, to, LightCount);
			delay(30);
		}

		int* tmp = to;
		to = from;
		from = tmp;
		printf("switch %d\n", count);
	}

	setAll(0, LightCount);

	free(colors);
	colors = 0;

	printf("Done!\n");

	return 0;
}


