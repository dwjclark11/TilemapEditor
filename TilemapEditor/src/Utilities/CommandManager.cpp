#include "CommandManager.h"
#include "../Logger/Logger.h"

CommandManager::~CommandManager()
{

}

void CommandManager::ExecuteCmd(std::shared_ptr<ICommand> command)
{
	// Here we are resetting the redostack to an empty CommandStack()
	mRedoStack = CommandStack();
	// Executes the desired command 
	command->Execute();
	// Pushed that command onto the undo stack
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

	// Call the redo function of the command that is on the top of the stack
	mRedoStack.top()->Redo();
	// Acts like an executed command, so we push back onto the undo stack
	mUndoStack.push(mRedoStack.top());
	// Pop that command off of the redo stack
	mRedoStack.pop();
	LOG_ERROR("RedoSize: {0}", mRedoStack.size());
}

void CommandManager::Clear()
{
	mUndoStack = {};
	mRedoStack = {};
}
