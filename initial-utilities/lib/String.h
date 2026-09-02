#pragma once
#include <stdio.h>
#include <string.h>
#include "DynamicArray.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
typedef enum {
	SUCCESS_CODE,
	FAIL_CODE,
	NUM_OF_CODES
} StatusCode;

typedef struct {
	uint64_t magic;
	char* base;
	size_t length;
}String;
// Use String when you want to do operations and to what really matters, string parameters should CStrings.

typedef String StringView; // This does not have any ownership and should not be used for allocating/freeing

typedef char* CStr;

#define WHITESPACE_STRING " \t\n"

#define NONASCII_BYTES_STRIP //create a range from 32 till 126

#define SetStringView_internal(view,b,l) (view).base = b; (view).length = l

#define STRING_MAGIC 0xf444f444f444f444

#define CONCAT_END NULL

ArrayDefine(size_t);

ArrayDefine(String);

ArrayDefine(StringView);

/*
* @brief Creates a String from CString, malloc'd
* @param nulled_string CString Source, type: CString
* @return String version of the CString passed, type: String
*/
String CStringToString(CStr);

/*
* @brief Creates a CString from String, malloc'd
* @param str String Source, type: String
* @return Null terminated CString from String passed, type: CString
*/
CStr StringToCString(String);

/*
* @brief Frees a string
* @param str String Source, type: String
* @return Nothing
*/
void StringFree(String);

/*
* @brief Checks if two strings are equal
* @param str1 String Source 1, type: String
* @param str2 String Source 2, type: String
* @return true or false based on equality, type: bool
*/
bool StringIsEqual(String*, String*);

/*
* @brief Trims both ends of a string by a strip set
* @param str String Source, type: String*
* @param stripped Stripped set, type: CString
* @return String copy of modified string, nondestructive
*/
String StringTrim(String*, CStr);

/*
* @brief Trims the right end of a string by a strip set
* @param str String Source, type: String*
* @param stripped Stripped set, type: CString
* @return String copy of modified string, nondestructive
*/
String StringTrimRight(String*, CStr);

/*
* @brief Trims the left end of a string by a strip set
* @param str String Source, type: String*
* @param stripped Stripped set, type: CString
* @return String copy of modified string, nondestructive
*/
String StringTrimLeft(String*, CStr);

/*
* @brief Concatenates two strings
* @param str1 String Source 1, type: String*
* @param str2 String Source 2, type: CString
* @return String copy of concatenated string
*/
String StringConcat(String*, String*);

/* Destructive Operation
* @brief Concatenates any number of strings, arguments can be CString or String
* @param str String Source, type: String*
* @param ... CString or String source, type: String* or CString
* @param CONCAT_END Marks the end of arguments, type: const
* @return returns str after modification, type: String, destructive
*/
String StringConcatMany(String*, ...);

/*
* @brief Replace substrings with a string with another substring, only takes String type
* @param str String Source type: String
* @param replacee String to be replaced, type: String
* @param replacer Replacing String, type: String
* @return String copy of modified string, type:String, nondestructive
*/
String StringReplace(String*, String*, String*);

/*
* @brief Slice a substring out string source
* @param str String Source, type: String*
* @param start Starting index, type: size_t
* @param end Ending index, type: size_t
* @return String View of string containing the substring, type: StringView, nondestructive
*/
StringView StringSlice(String*, size_t, size_t);

/*
* @brief Find the index of first occurrence of string
* @param str String Source, type: String*
* @param target Target String, type: String*
* @return index of first occurrence of target, type: int
*/
int StringFindFirstOccurence(String*, String*);

/*
* @brief Find all indices where target is found
* @param str String Source, type: String*
* @param target Target String, type: String*
* @return Dynamic array of indices where target is found, type: size_tArray
*/
size_tArray StringFindAllOccurrences(String*, String*);

/*
* @brief Find all indices where target is found
* @param str String Source, type: String*
* @param target Target String, type: CString
* @return Dynamic array of indices where target is found, type: size_tArray
*/
size_tArray StringFindAllOccurrencesCStringTarget(String*, CStr);

/*
* @brief Split string into an array of views based of a single character delimiter
* @param str String Source, type: String*
* @param delim Delimiting character, type: CString
* @return Dynamic array of views of split string, type: StringViewArray, nondestructive
*/
StringViewArray StringSplit(String*, CStr);

/*
* @brief Split string into an array of views based of a delimiter set, good for tokenizing
* @param str String Source, type: String*
* @param delim Delimiting set, type: CString
* @return Dynamic array of views of split string, type: StringViewArray, nondestructive
*/
StringViewArray StringTokenize(String*, CStr);

/*
* @brief Create new copy of string, uses malloc and memset so it must be freed when unnecessary
* @param str_src String Source, type: String*
* @return String copy of source, type: String
*/
String StringCreateCopy(String*);

/*
* WIP
* @brief Joins a set of String Views by a delimiter
* @param views Split up string views type: StringViewArray
* @param replacee String to be replaced, type: CString
* @return String copy of modified string, type: String, nondestructive
*/
String StringJoin(StringViewArray, CStr);

/*
* @brief Replace substrings with a string with another substring, only takes CString type
* @param str String Source type: String
* @param replacee String to be replaced, type: CString
* @param replacer Replacing String, type: CString
* @return String copy of modified string, type: String, nondestructive
*/
String StringReplaceCStringTargets(String*, CStr, CStr);