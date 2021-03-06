/*
	Author: bitluni 2019
	License: 
	Creative Commons Attribution ShareAlike 4.0
	https://creativecommons.org/licenses/by-sa/4.0/
	
	For further details check out: 
		https://youtube.com/bitlunislab
		https://github.com/bitluni
		http://bitluni.net
*/
#pragma once
#include "Graphics.h"

class GraphicsR5G5B4A1X10S2Swapped: public Graphics<unsigned short>
{
	public:
	typedef unsigned short Color;
	static const Color RGBAXMask = 0x3fff;
	Color SBits;
	
	GraphicsR5G5B4A1X10S2Swapped()
	{
		SBits = 0xc000;
		frontColor = 0xf;
	}

	virtual int R(Color c) const
	{
		return (c & 1) * 255;
	}
	virtual int G(Color c) const
	{
		return (c & 2) ? 255 : 0;
	}
	virtual int B(Color c) const
	{
		return (c & 4) ? 255 : 0;
	}
	virtual int A(Color c) const
	{
		return (c & 8) ? 255 : 0;
	}

	virtual Color RGBA(int r, int g, int b, int a = 255) const
	{
		return ((r >> 7) & 1) | ((g >> 6) & 2) | ((b >> 5) & 4) | ((a >> 4) & 8);
	}

	virtual void dotFast(int x, int y, Color color)
	{
		backBuffer[y][x^1] = (color & RGBAXMask) | SBits;
	}

	virtual void dot(int x, int y, Color color)
	{
		if ((unsigned int)x < xres && (unsigned int)y < yres)
			backBuffer[y][x^1] = (color & RGBAXMask) | SBits;
	}

	virtual void dotAdd(int x, int y, Color color)
	{
		if ((unsigned int)x < xres && (unsigned int)y < yres)
			backBuffer[y][x^1] = backBuffer[y][x^1] | (color & RGBAXMask);
	}
	
	virtual void dotMix(int x, int y, Color color)
	{
		if ((unsigned int)x < xres && (unsigned int)y < yres && (color & 8) != 0)
			backBuffer[y][x^1] = (color & RGBAXMask) | SBits;
	}
	
	virtual Color get(int x, int y)
	{
		if ((unsigned int)x < xres && (unsigned int)y < yres)
			return backBuffer[y][x^1] & RGBAXMask;
		return 0;
	}

	virtual void clear(Color color = 0)
	{
		for (int y = 0; y < this->yres; y++)
			for (int x = 0; x < this->xres; x++)
				backBuffer[y][x^1] = (color & RGBAXMask) | SBits;
	}

	virtual Color** allocateFrameBuffer()
	{
		Color** frame = (Color **)malloc(yres * sizeof(Color *));
		for (int y = 0; y < yres; y++)
		{
			frame[y] = (Color *)malloc(xres * sizeof(Color));
			for (int x = 0; x < xres; x++)
				frame[y][x] = SBits;
		}
		return frame;
	}
};