#pragma once
#include "ICommand.h"
#include <vector>

class CommandManager
{
private:
	CommandStack mUndoStack;
	CommandStack mRedoStack;

public:
	CommandManager() {};
	~CommandManager();

	void ExecuteCmd(std::shared_ptr<ICommand> command);
	void Undo();
	void Redo();

};