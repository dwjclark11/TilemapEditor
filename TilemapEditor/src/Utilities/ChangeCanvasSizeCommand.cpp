#include "ChangeCanvasSizeCommand.h"
#include "../Canvas.h"
#include "../Logger/Logger.h"


ChangeCanvasSizeCommand::ChangeCanvasSizeCommand(std::shared_ptr<class Canvas>& canvas, const int& prevCanvasWidth, const int& prevCanvasHeight)
	: mCanvas(canvas)
	, mPrevCanvasWidth(prevCanvasWidth)
	, mPrevCanvasHeight(prevCanvasHeight)
{

}

void ChangeCanvasSizeCommand::Execute()
{
	// Nothing to set
	//LOG_INFO("width: {0}\nprevWidth: {1}\nheight: {2}\nprevHeight: {3}", mCanvasWidth, mPrevCanvasWidth, mCanvasHeight, mPrevCanvasHeight);
}

void ChangeCanvasSizeCommand::Undo()
{
	// The data is swapped for width and height
	int tempWidth = mCanvas->GetWidth();
	mCanvas->SetWidth(mPrevCanvasWidth);
	mPrevCanvasWidth = tempWidth;

	int tempHeight = mCanvas->GetHeight();
	mCanvas->SetHeight(mPrevCanvasHeight);
	mPrevCanvasHeight = tempHeight;
}

void ChangeCanvasSizeCommand::Redo()
{
	// This just calls the undo function
	Undo();
}
