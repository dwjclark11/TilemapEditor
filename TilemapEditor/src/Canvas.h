#pragma once

class Canvas
{
private:
	int mWidth;
	int mHeight;

public:
	Canvas(int width = 0, int height = 0)
	{
		mWidth = width;
		mHeight = height;
	}
	~Canvas() {}

	inline const int& GetWidth() const { return mWidth; }
	inline const int& GetHeight() const { return mHeight; }
	inline void SetWidth(int width) { mWidth = width; }
	inline void SetHeight(int height) { mHeight = height; }
};