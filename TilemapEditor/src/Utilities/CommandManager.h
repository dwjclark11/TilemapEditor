#pragma once
#include "Utilities/ICommand.h"
#include <vector>

/*
 *  The CommandManager keeps track of all commands that have been executed
 *  The executed commands need to be a child of the type ICommand
 */
class CommandManager
{
  private:
	// Keep track of all actions so we can undo if necessary
	CommandStack mUndoStack;
	// Keep track of all undone actions, so we can redo if necessary,
	// This is reset after ever new execution of a command
	CommandStack mRedoStack;

  public:
	CommandManager(){};
	~CommandManager();

	/*
	 *  ExecuteCmd - This resets the RedoStack and executes the command
	 *  based on the passed in ICommand child.
	 *  @params - takes in a shared_ptr to an ICommand object
	 */
	void ExecuteCmd( std::shared_ptr<ICommand> command );

	/*
	 *  If the mUndoStack is not empty, Calls the undo function for the command that is on the top of mUndoStack.
	 *  Pushes that command onto the Redo stack, then pops the command off of the undo stack
	 */
	void Undo();

	/*
	 *  If the mRedoStack is not empty, Calls the redo function for the command that is on the top of mRedoStack.
	 *  Pushes that command onto the Undo stack, then pops the command off of the undo stack
	 */
	void Redo();

	// Test to clear stacks
	void Clear();
};