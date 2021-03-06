//This example shows how a custom VGA resolution can be created for one of the base modes
//You need to connect a VGA screen cable to the pins specified below.
//cc by-sa 4.0 license
//bitluni

//including the needed header
#include <ESP32Lib.h>
#include <Ressources/Font6x8.h>

//pin configuration
const int redPin = 14;
const int greenPin = 19;
const int bluePin = 27;
const int hsyncPin = 0;
const int vsyncPin = 5;

//VGA Device using an interrupt to unpack the pixels from 4bit to 16bit for the I²S
//This takes some CPU time in the background but is able to fit a frame buffer in the memory
VGA3BitI vga;

void setup()
{
	//enabling double buffering
	vga.setFrameBufferCount(2);
	//let's calculate the max horizontal resolution we can get from a given mode
	int maxX = vga.MODE800x600.maxXRes();
	//Mode::custom(xres, yres, fixedYDivider = 1) calculates the parameters for our custom resolution.
	//the y resolution is only scaling integer divisors (yet).
	//if you don't like to let it scale automatically pass a fixed parameter with a fixed divider.
	Serial.begin(115200);
	vga.MODE800x600.custom(maxX, 300).print<HardwareSerial>(Serial);
	vga.init(vga.MODE800x600.custom(maxX, 300), redPin, greenPin, bluePin, hsyncPin, vsyncPin);
	//setting the font
	vga.setFont(Font6x8);
}

///draws a bouncing balls
void balls()
{
	//some basic gravity physics
	static VGA3BitI::Color c[4] = {vga.RGB(0, 255, 0), vga.RGB(0, 255, 255), vga.RGB(255, 0, 255), vga.RGB(255, 255, 0)};
	static float y[4] = {400, 400, 400, 400};
	static float x[4] = {200, 200, 200, 200};
	static float vx[4] = {10, -7, 5, -3};
	static float vy[4] = {0, 1, 2, 3};
	static unsigned long lastT = 0;
	unsigned long t = millis();
	float dt = (t - lastT) * 0.001f;
	lastT = t;
	const int r = 20;
	for (int i = 0; i < 4; i++)
	{
		int rx = r;
		int ry = r;
		vy[i] += -9.81f * dt * 100;
		x[i] += vx[i];
		y[i] += vy[i] * dt;
		//check for boundaries and bounce back
		if (y[i] < r && vy[i] < 0)
		{
			vy[i] = 600 + i * 50;
			ry = y[i];
		}
		if (x[i] < r && vx[i] < 0)
		{
			vx[i] = -vx[i];
			rx = x[i];
		}
		if (x[i] >= vga.xres - r && vx[i] > 0)
		{
			vx[i] = -vx[i];
			rx = vga.xres - x[i];
		}
		//draw a filled ellipse
		vga.fillEllipse(x[i], vga.yres - y[i] - 1, rx, ry, c[i]);
		vga.ellipse(x[i], vga.yres - y[i] - 1, rx, ry, 0);
	}
}

//mainloop
void loop()
{
	//draw a background
	for (int y = 0; y * 10 < vga.yres; y++)
		for (int x = 0; x * 10 < vga.xres; x++)
			vga.fillRect(x * 10, y * 10, 10, 10, (x + y) & 1 ? vga.RGB(255, 0, 0) : vga.RGB(255, 255, 255));
	//text position
	vga.setCursor(10, 10);
	//black text color no background color
	vga.setTextColor(vga.RGB(0));
	//show the remaining memory
	vga.print(vga.xres);
	vga.print("x");
	vga.println(vga.yres);
	vga.print("free memory: ");
	vga.print((int)heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
	//draw bouncing balls
	balls();
	//show the backbuffer (only needed when using backbuffering)
	vga.show();
}