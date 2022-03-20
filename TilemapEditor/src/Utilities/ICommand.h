#pragma once
#include <memory>
#include <stack>

class ICommand
{
public:
	virtual ~ICommand() { }
	virtual void Execute() = 0;
	virtual void Undo() = 0;
	virtual void Redo() = 0;


};

typedef std::stack<std::shared_ptr<ICommand>> CommandStack;

#include "AddTileCommand.h"
