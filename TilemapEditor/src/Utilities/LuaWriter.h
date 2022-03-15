#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cassert>

/*
	This Class is to help write formatted lua tables to a text file
	using fstream
*/

class LuaWriter
{
private:
	char mSeparator;
	bool mValueWritten, mNewLine, mError, mMinimize, mSuppress;
	int mIndent;

	template <typename T>
	void Write(const T value, std::fstream& file);

	void WriteIndent(std::fstream& file);
	void WriteNewLine(std::fstream& file);
	void PrepareNewValue(std::fstream& file);

public:
	LuaWriter();
	~LuaWriter() = default;

	void WriteStartDocument();
	void WriteEndDocument(std::fstream& file);
	void WriteCommentSeparation(std::fstream& file);
	void WriteCommentLine(const std::string& comment, std::fstream& file);
	void WriteStartReturnTable(std::fstream& file);
	void WriteDeclareTable(const std::string name, std::fstream& file);

	void WriteEndTable(std::fstream& file);
	void WriteEndTable(bool sameLine, std::fstream& file);
	void WriteEndTableWithSeparator(bool sameLine, std::fstream& file);

	void PrepareNewLine(std::fstream& file);

	inline void SetSuppression(bool suppress) { mSuppress = suppress; }
	inline const bool IsSuppressed() const { return mSuppress; }
	inline void SetMinimize(bool minimize) { mMinimize = minimize; }
	inline const bool IsMinimized() const { return mMinimize; }

	/*
		This function takes a string and returns a new "Quoted" string
	*/

	std::string Quote(const std::string& quote);

	// Template functions for entering values and keys 
		// Template functions for enter values/keys of different types --> Generic
	template <typename TValue> void WriteValue(TValue value, std::fstream& file);
	template <typename TName> void WriteStartTable(TName name, bool quoted, std::fstream& file);
	template <typename TValue> void WriteUnquotedValue(TValue value, std::fstream& file);
	template <typename TValue> void WriteUnquotedValue(TValue value, bool newLine, std::fstream& file);
	template <typename TKey, typename TValue> void WriteQuotedKeyAndValue(TKey key, TValue value, std::fstream& file);
	template <typename TKey, typename TValue> void WriteKeyAndValue(TKey key, TValue value, std::fstream& file);
	template <typename TKey, typename TValue> void WriteKeyAndValue(TKey key, TValue value, bool finalValue, std::fstream& file);
	template <typename TKey, typename TValue> void WriteKeyAndUnquotedValue(TKey key, TValue value, std::fstream& file, bool sameLine = false, bool lastValue = false);
	template <typename TKey, typename TValue> void WriteKeyAndQuotedValue(TKey key, TValue value, std::fstream& file);
};


// Template Initializations
template<typename T>
void LuaWriter::Write(const T value, std::fstream& fileName)
{
	fileName << value;
}

template<typename TValue>
void LuaWriter::WriteValue(TValue value, std::fstream& file)
{
	WriteUnquotedValue(value, file);
}

template<typename TName>
void LuaWriter::WriteStartTable(TName name, bool quoted, std::fstream& file)
{
	PrepareNewLine(file);
	Write('[', file);
	if (quoted)
		//Write(Quote(name), file);
		std::cout << "quoted";
	else
		Write(name, file);

	Write(mMinimize ? "]={" : "] = {", file);
	++mIndent;
	mNewLine = false;
	mValueWritten = false;
}

template<typename TValue>
void LuaWriter::WriteUnquotedValue(TValue value, std::fstream& file)
{
	PrepareNewValue(file);
	Write(value, file);
	mNewLine = false;
	mValueWritten = true;
}

template<typename TValue>
void LuaWriter::WriteUnquotedValue(TValue value, bool newLine, std::fstream& file)
{
	if (newLine)
		PrepareNewValue(file);

	Write(value, file);
	mNewLine = newLine;
	mValueWritten = true;
}

template<typename TKey, typename TValue>
void LuaWriter::WriteQuotedKeyAndValue(TKey key, TValue value, std::fstream& file)
{
	PrepareNewLine(file);
	Write('[', file);
	Write(Quote(key), file);
	Write(mMinimize ? "]=" : "] = ", file);
	Write(value, file);

}

template<typename TKey, typename TValue>
void LuaWriter::WriteKeyAndValue(TKey key, TValue value, std::fstream& file)
{
	PrepareNewLine(file);
	Write(' ', file);
	Write(key, file);
	Write(mMinimize ? "=" : " = ", file);
	Write('"', file);
	Write(value, file);
	//Write(mSeparator, file);
	mNewLine = false;
	mValueWritten = true;
}

template<typename TKey, typename TValue>
void LuaWriter::WriteKeyAndValue(TKey key, TValue value, bool finalValue, std::fstream& file)
{
	//PrepareNewLine(file);
	Write(' ', file);
	Write(key, file);
	Write(mMinimize ? "=" : " = ", file);
	//Write('"', file);
	Write(value, file);
	if (!finalValue)
		Write(mSeparator, file);

	mNewLine = false;
	mValueWritten = true;
}

template<typename TKey, typename TValue>
void LuaWriter::WriteKeyAndUnquotedValue(TKey key, TValue value, std::fstream& file, bool sameLine, bool lastValue)
{
	if (!sameLine)
		PrepareNewLine(file);

	Write(key, file);
	Write(mMinimize ? "=" : " = ", file);
	Write(value, file);

	// Separate the values
	if (sameLine && !lastValue)
		Write(mSeparator, file);

	mNewLine = false;
	mValueWritten = true;
}

template<typename TKey, typename TValue>
void LuaWriter::WriteKeyAndQuotedValue(TKey key, TValue value, std::fstream& file)
{
	PrepareNewLine(file);
	Write(key, file);
	Write(mMinimize ? "=" : " = ", file);
	Write(Quote(value), file);
	//Write('"', file);
	mNewLine = false;
	mValueWritten = true;
}