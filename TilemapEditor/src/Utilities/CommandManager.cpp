#include "CommandManager.h"
#include "../Logger/Logger.h"

CommandManager::~CommandManager()
{

}

void CommandManager::ExecuteCmd(std::shared_ptr<ICommand> command)
{
	mRedoStack = CommandStack();
	command->Execute();
	mUndoStack.push(command);
}

void CommandManager::Undo()
{
	// If the undo stack is empty, there is nothing to do
	if (mUndoStack.size() <= 0)
	{
		LOG_ERROR("Undo Stack Empty!");
		return;
	}
		

	// Undo the most recently executed command
	mUndoStack.top()->Undo();
	// Push that command onto the redo stack
	mRedoStack.push(mUndoStack.top());
	// Pop off that undo command
	mUndoStack.pop();
}

void CommandManager::Redo()
{
	// If the redo stack is empty, there is nothing to do
	if (mRedoStack.size() <= 0)
	{
		LOG_ERROR("Redo Stack Empty!");
		return;
	}
		
	
	mRedoStack.top()->Redo();
	mUndoStack.push(mRedoStack.top());
	mRedoStack.pop();
	LOG_ERROR("RedoSize: {0}", mRedoStack.size());
}
