#include "Utilities/LuaWriter.h"

void LuaWriter::WriteIndent( std::fstream& file )
{
	// This will write indents based on the current level
	// Or how deep it has to go for the tables
	for ( int level = mIndent; level; --level )
	{
		Write( "    ", file );
	}
}

void LuaWriter::WriteNewLine( std::fstream& file )
{
	if ( !mNewLine )
	{
		if ( !mMinimize )
		{
			if ( mSuppress )
			{
				Write( ' ', file );
			}
			else
			{
				Write( '\n', file );
				WriteIndent( file );
			}
		}
	}
}

void LuaWriter::PrepareNewValue( std::fstream& file )
{
	if ( !mValueWritten )
	{
		WriteNewLine( file );
	}
	else
	{
		Write( mSeparator, file );
		if ( !mMinimize )
		{
			Write( ' ', file );
		}
	}
}

LuaWriter::LuaWriter()
	: mSeparator( ',' )
	, mValueWritten( false )
	, mMinimize( false )
	, mNewLine( false )
	, mSuppress( false )
	, mError( false )
	, mIndent( 0 )
{
}

void LuaWriter::WriteStartDocument()
{
	// Indent should be zero when starting a new Document
	assert( mIndent == 0 );
}

void LuaWriter::WriteEndDocument( std::fstream& file )
{
	assert( mIndent == 0 );
	// We always want to leave an extra line on the bottom of the file
	Write( '\n', file );
}

void LuaWriter::WriteCommentSeparation( std::fstream& file )
{
	Write( "----------------------------------------------------------", file );
	Write( '\n', file );
}

void LuaWriter::WriteCommentLine( const std::string& comment, std::fstream& file )
{
	// Start the comment Lua comment -- or --[[ block comment ]]
	Write( "-- " + comment, file );
	// Write a new line
	Write( '\n', file );
}

void LuaWriter::WriteStartReturnTable( std::fstream& file )
{
	PrepareNewLine( file );
	Write( '{', file );
	++mIndent;
	mNewLine = false;
	mValueWritten = false;
}

void LuaWriter::WriteDeclareTable( const std::string name, std::fstream& file )
{
	PrepareNewLine( file );
	Write( name, file );
	Write( mMinimize ? "={" : " = {", file );
	++mIndent;
	mNewLine = false;
	mValueWritten = false;
}

void LuaWriter::WriteEndTable( std::fstream& file )
{
	PrepareNewLine( file );
	--mIndent;
	if ( mValueWritten )
		WriteNewLine( file );

	Write( '}', file );
	mNewLine = false;
	mValueWritten = true;
}

void LuaWriter::WriteEndTable( bool sameLine, std::fstream& file )
{
	--mIndent;
	if ( mValueWritten && !sameLine )
		WriteNewLine( file );

	Write( '}', file );
	mNewLine = false;
	mValueWritten = true;
}

void LuaWriter::WriteEndTableWithSeparator( bool sameLine, std::fstream& file )
{
	--mIndent;
	if ( mValueWritten && !sameLine )
		WriteNewLine( file );

	Write( '}', file );
	Write( mSeparator, file );
	mNewLine = false;
	mValueWritten = true;
}

void LuaWriter::PrepareNewLine( std::fstream& file )
{
	if ( mValueWritten )
	{
		Write( mSeparator, file );
		mValueWritten = false;
	}
	WriteNewLine( file );
}

std::string LuaWriter::Quote( const std::string& quote )
{
	std::string quoted;
	// We want to reserve space for the string + 2 quotation marks
	quoted.reserve( quote.length() + 2 );
	quoted += '"';

	for ( const char ch : quote )
	{
		// These are special cases that must be handled correctly based on key characters
		switch ( ch )
		{
		case '\\': quoted.append( "\\\\" ); break;
		case '"': quoted.append( "\\\"" ); break;
		case '\n': quoted.append( "\\n" ); break;
		default: quoted += ch;
		}
	}

	quoted += '"';
	return quoted;
}
