/*
	Author: bitluni 2019
	License: 
	Creative Commons Attribution ShareAlike 2.0
	https://creativecommons.org/licenses/by-sa/2.0/
	
	For further details check out: 
		https://youtube.com/bitlunislab
		https://github.com/bitluni
		http://bitluni.net
*/
#pragma once
#include "Graphics.h"
#include "TriangleTree.h"

class GraphicsR5G5B4A2: public Graphics<unsigned short>
{
	public:
	typedef unsigned short Color;
	Color **frame;
	Color **backbuffer;

	virtual void dotFast(int x, int y, Color color)
	{
		backbuffer[y][x] = color;
	}

	virtual void dot(int x, int y, Color color)
	{
		if ((unsigned int)x < xres && (unsigned int)y < yres)
			backbuffer[y][x] = color;
	}

	virtual void dotAdd(int x, int y, Color color)
	{
		//todo repair this
		if ((unsigned int)x < xres && (unsigned int)y < yres)
			backbuffer[y][x] = color + backbuffer[y][x];
	}
	
	virtual void dotMix(int x, int y, Color color)
	{
		if ((unsigned int)x < xres && (unsigned int)y < yres && (color >> 14) != 0)
		{
			unsigned int ai = (3 - (color >> 14)) * (65536 / 3);
			unsigned int a = 65536 - ai;
			unsigned int co = backbuffer[y][x];
			unsigned int ro = (co & 0b11111) * ai;
			unsigned int go = (co & 0b1111100000) * ai;
			unsigned int bo = (co & 0b11110000000000) * ai;
			unsigned int r = (color & 0b11111) * a + ro;
			unsigned int g = ((color & 0b1111100000) * a + go) & 0b11111000000000000000000000;
			unsigned int b = ((color & 0b11110000000000) * a + bo) & 0b111100000000000000000000000000;
			backbuffer[y][x] = (r | g | b) >> 16;
		}	
	}
	
	virtual char get(int x, int y)
	{
		if ((unsigned int)x < xres && (unsigned int)y < yres)
			return backbuffer[y][x];
		return 0;
	}

	virtual void clear(Color clear = 0)
	{
		for (int y = 0; y < this->yres; y++)
			for (int x = 0; x < this->xres; x++)
				this->backbuffer[y][x] = clear;
	}

	virtual void show()
	{
		Color **b = backbuffer;
		backbuffer = frame;
		frame = b;
	}

	virtual void initBuffers(const bool doubleBuffer = true, const bool zbuffer = false)
	{
		frame = (Color **)malloc(yres * sizeof(Color *));
		if(doubleBuffer)
			backbuffer = (Color **)malloc(yres * sizeof(Color *));
		else
			backbuffer = frame;
		//not enough memory for z-buffer implementation
		//zbuffer = (char**)malloc(yres * sizeof(char*));
		for (int y = 0; y < yres; y++)
		{
			frame[y] = (Color *)malloc(xres * sizeof(Color));
			if(doubleBuffer)
				backbuffer[y] = (Color *)malloc(xres * sizeof(Color));
			//zbuffer[y] = (char*)malloc(xres);
		}
	}
};