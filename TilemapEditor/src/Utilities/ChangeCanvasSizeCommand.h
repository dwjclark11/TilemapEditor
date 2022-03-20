#pragma once
#include "ICommand.h"

class ChangeCanvasSizeCommand : public ICommand
{
private:
	std::shared_ptr<class Canvas> mCanvas;

	int mPrevCanvasWidth;
	int mPrevCanvasHeight;
	
public:
	ChangeCanvasSizeCommand(std::shared_ptr<class Canvas>& canvas, const int& prevCanvasWidth, const int& prevCanvasHeight);
	virtual void Execute() override;
	virtual void Undo() override;
	virtual void Redo() override;

};