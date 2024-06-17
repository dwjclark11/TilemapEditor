#pragma once
#include <memory>
#include <stack>

/*
 *  ICommand is an interface that is used for all of the Undo/Redo functions
 *
 */
class ICommand
{
  public:
	virtual ~ICommand() {}
	virtual void Execute() = 0;
	virtual void Undo() = 0;
	virtual void Redo() = 0;
};

/*
 *  Typedef to be used in the CommandManager. We will create two stacks
 *  CommandStack mUndoStack
 *  CommandStack mRedoStack
 *  These will help to keep track of the actions that the user is doing so we can Undo/Redo those actions if needed
 */
typedef std::stack<std::shared_ptr<ICommand>> CommandStack;

#include "AddTileCommand.h"
#include "RemoveTileCommand.h"
#include "ChangeCanvasSizeCommand.h"