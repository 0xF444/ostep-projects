#include "String.h"
#include <string.h>
#include <stdlib.h>

bool IsCharInCString_internal(char selected_char, CStr stripped_set)
{
	for (size_t i = 0; i < strlen(stripped_set); i++)
	{
		if (selected_char == stripped_set[i]) return true;
	}
	return false;
}

String CStringToString(CStr nulled_str)
{
	String str = { 0 };
	str.magic = STRING_MAGIC;
	str.length = strlen(nulled_str);
	char* base = (char*)malloc(str.length);
	if (!base) { return str; }
	memcpy(base, nulled_str, str.length);
	str.base = base;
	return str;
}

CStr StringToCString(String str)
{
	char* returned_cstr = (char*)malloc(str.length + 1);
	if (!returned_cstr) { return NULL; }
	memcpy(returned_cstr, str.base, str.length);
	returned_cstr[str.length] = '\0';
	return returned_cstr;
}

String StringConcat(String* str1, String* str2)
{
	String result_string = { 0 };
	result_string.length = str1->length + str2->length;
	char* concatted_buffer = (char*)malloc(str1->length + str2->length);
	if (!concatted_buffer) return *str1; // return str1 when failed

	result_string.base = concatted_buffer;
	memcpy(result_string.base, str1->base, str1->length); // Copy the first string
	memcpy(result_string.base + str1->length, str2->base, str2->length); // Copy the second string
	return result_string;

}

void StringConcatOverwriteOriginal_internal(String* str1, String* str2)
{
	size_t original_length = str1->length;
	size_t new_length = str1->length + str2->length;
	char* concatted_buffer = (char*)realloc(str1->base, new_length);
	if (!concatted_buffer) return;
	str1->base = concatted_buffer;        // update base pointer
	str1->length = new_length;            // update length
	memcpy(str1->base + original_length, str2->base, str2->length);
}
String StringConcatMany(String* str, ...)
{
	va_list args_list;
	va_start(args_list, str);

	void* arg_ptr;
	while ((arg_ptr = va_arg(args_list, void*)) != CONCAT_END)
	{
		if (*(uint64_t*)arg_ptr == STRING_MAGIC)
		{
			StringConcatOverwriteOriginal_internal(str, (String*)arg_ptr);
		}
		else
		{
			char* cstr = (char*)arg_ptr;
			size_t cstr_length = strlen(cstr);
			size_t new_length = str->length + cstr_length;
			char* concatted_buffer = (char*)realloc(str->base, new_length);
			if (!concatted_buffer) break;
			str->base = concatted_buffer;
			memcpy(str->base + str->length, cstr, cstr_length);
			str->length = new_length;
		}
	}
	va_end(args_list);
	return *str;
}

StringView StringSlice(String* str, size_t start, size_t end)
{
	StringView returned_str_view = { 0 };
	SetStringView_internal(returned_str_view, str->base, str->length);
	if (start > end || end > str->length) return returned_str_view;
	size_t delta = end - start; // safe to compute only after validation
	SetStringView_internal(returned_str_view, str->base + start, delta);
	return returned_str_view;
}

int StringFindFirstOccurence(String* str, String* target)
{
	int i, j, k;
	int indx_accum = -1;
	for (i = 0; i < str->length; i++)
	{
		for (j = i, k = 0; k < target->length && str->base[j] == target->base[k]; j++, k++)
			; // Attempts to match the target into the source, character by character.
		if (k > 0 && k == (target->length)) // Check if has a nonzero value (a single character was found) AND k reached the end of target, meaning a full match.
			indx_accum = i; // if we return directly, we find the first occurrence
		// We can keep accumulating, thus finding the rightmost occurrence.
	}
	return indx_accum;
}

size_tArray StringFindAllOccurrences(String* str, String* target)
{
	size_tArray occurences_indices = { 0 };
	int i, j, k;
	for (i = 0; i < str->length; i++)
	{
		for (j = i, k = 0; k < target->length && str->base[j] == target->base[k]; j++, k++)
			; // Attempts to match the target into the source, character by character.
		if (k > 0 && k == (target->length)) // Check if has a nonzero value (a single character was found) AND k reached the end of target, meaning a full match.
			ArrayAppend(size_t, occurences_indices, (size_t)i);
		// We can keep accumulating, thus finding the rightmost occurrence.
	}
	return occurences_indices;
}
size_tArray StringFindAllOccurrencesCStringTarget(String* str, CStr target)
{
	size_tArray occurences_indices = { 0 };
	int i, j, k;
	for (i = 0; i < str->length; i++)
	{
		for (j = i, k = 0; target[k] != '\0' && str->base[j] == target[k]; j++, k++)
			; // Attempts to match the target into the source, character by character.
		if (k > 0 && target[k] == '\0') // Check if has a nonzero value (a single character was found) AND k reached the end of target, meaning a full match.
			ArrayAppend(size_t, occurences_indices, (size_t)i);
		// We can keep accumulating, thus finding the rightmost occurrence.
	}
	return occurences_indices;
}

StringViewArray StringSplit(String* str, CStr delim)
{
	StringViewArray views = { 0 };
	size_tArray occurrences = StringFindAllOccurrencesCStringTarget(str, delim);

	size_t delim_len = strlen(delim);
	size_t prev = 0;

	for (int i = 0; i < occurrences.count; i++)
	{
		size_t delim_pos = occurrences.items[i];

		if (delim_pos != prev) // If current delim is not equal to prev base, we have ourselves a token
		{
			StringView view = { 0 };
			view.base = str->base + prev;
			view.length = delim_pos - prev;
			ArrayAppend(StringView, views, view);
		}

		prev = delim_pos + delim_len; // Skip entire delim found
	}

	if (prev < str->length) // if a remaining token is left after all occurrences have been consumed
	{
		StringView view = { 0 };
		view.base = str->base + prev;
		view.length = str->length - prev; // length is now the end of the string.
		ArrayAppend(StringView, views, view);
	}

	return views;
}
StringViewArray StringTokenize(String* str, CStr delimiters)
{
	StringViewArray views = { 0 };
	size_t prev = 0;

	for (size_t i = 0; i < str->length; i++)
	{
		if (IsCharInCString_internal(str->base[i], delimiters))
		{
			if (i != prev) // skip empty tokens
			{
				StringView view = { 0 };
				view.base = str->base + prev;
				view.length = i - prev;
				ArrayAppend(StringView, views, view);
			}
			prev = i + 1;
		}
	}

	// trailing token
	if (prev < str->length)
	{
		StringView view = { 0 };
		view.base = str->base + prev;
		view.length = str->length - prev;
		ArrayAppend(StringView, views, view);
	}

	return views;
}

String StringCreateCopy(String* str_src)
{
	String copy = { .base = (char*)malloc(str_src->length),.length = str_src->length };
	if (copy.base == 0) return *str_src;
	memcpy(copy.base, str_src->base, str_src->length);
	return copy;
}

void StringFree(String str)
{
	free(str.base);
}

bool StringIsEqual(String* str1, String* str2)
{
	if (str1->length != str2->length) return false;
	for (size_t i = 0; i < str1->length; i++)
	{
		if (str1->base[i] != str2->base[i]) return false;
	}
	return true;
}

String StringTrim(String* str, CStr stripped)
{
	String trimmed_str;
	char* new_base_indx = str->base;
	size_t new_length = str->length;
	bool table[256] = { 0 };
	for (int i = 0; i < strlen(stripped); i++)
	{
		table[(unsigned char)stripped[i]] = true;
	}
	while (new_length > 0 && table[(unsigned char)*(new_base_indx + new_length - 1)])
	{
		new_length--;
	}// Right Trim
	while (new_length > 0 && table[(unsigned char)*(new_base_indx)])
	{
		new_base_indx++;
		new_length--;
	}// Left Trim
	char* new_base = (char*)malloc(new_length);
	if (!new_base) return *str; // Failed, return original copy
	memcpy(new_base, new_base_indx, new_length);
	trimmed_str.base = new_base;
	trimmed_str.length = new_length;
	return trimmed_str;

}

String StringTrimLeft(String* str, CStr stripped)
{
	String trimmed_str;
	char* new_base_indx = str->base;
	size_t new_length = str->length;
	bool table[256] = { 0 };
	for (int i = 0; i < strlen(stripped); i++)
	{
		table[(unsigned char)stripped[i]] = true;
	}
	while (new_length > 0 && table[(unsigned char)*(new_base_indx)])
	{
		new_base_indx++;
		new_length--;
	}// Left Trim
	char* new_base = (char*)malloc(new_length);
	if (!new_base) return *str; // Failed, return original copy
	memcpy(new_base, new_base_indx, new_length);
	trimmed_str.base = new_base;
	trimmed_str.length = new_length;
	return trimmed_str;

}

String StringTrimRight(String* str, CStr stripped)
{
	String trimmed_str;
	char* new_base_indx = str->base;
	size_t new_length = str->length;
	bool table[256] = { 0 };
	for (int i = 0; i < strlen(stripped); i++)
	{
		table[(unsigned char)stripped[i]] = true;
	}
	while (new_length > 0 && table[(unsigned char)*(new_base_indx + new_length - 1)])
	{
		new_length--;
	}// Right Trim
	char* new_base = (char*)malloc(new_length);
	if (!new_base) return *str; // Failed, return original copy
	memcpy(new_base, new_base_indx, new_length);
	trimmed_str.base = new_base;
	trimmed_str.length = new_length;
	return trimmed_str;
}

String StringReplace(String* str, String* replacee, String* replacer)
{
	/* Replacee is in str, replacer should be in the str_copy buffer for each replacee occurrence in str. */
	String str_copy = { 0 };
	size_tArray occurrence = { 0 };
	occurrence = StringFindAllOccurrences(str, replacee);
	if (occurrence.count == 0) return *str; // return the same string, no matches were found
	ptrdiff_t delta = (ptrdiff_t)replacer->length - (ptrdiff_t)replacee->length; //  
	str_copy.length = (size_t)((ptrdiff_t)str->length + (ptrdiff_t)occurrence.count * delta);
	str_copy.base = (char*)malloc(str_copy.length);
	// Copying mechanism
	for (size_t copyindx = 0, occurindx = 0, srcindx = 0; copyindx < str_copy.length && srcindx < str->length; copyindx++, srcindx++)
	{
		if (occurindx < occurrence.count && srcindx == occurrence.items[occurindx])
		{
			memcpy(str_copy.base + copyindx, replacer->base, replacer->length);
			copyindx += replacer->length - 1; // Since it is replaced, increase the copying index by the size of replacer.
			srcindx += replacee->length - 1; // Since the replacee is in the original string, increase the srcindx by replacee.
			occurindx++;
			continue;
			/*The indicies lose their sync the moment we replace */
		}
		str_copy.base[copyindx] = str->base[srcindx];
	}
	return str_copy;
}

String StringReplaceCStringTargets(String* str, CStr replacee, CStr replacer)
{
	/* Replacee is in str, replacer should be in the str_copy buffer for each replacee occurrence in str. */
	String str_copy = { 0 };
	size_tArray occurrence = { 0 };
	size_t replacer_len = strlen(replacer);
	size_t replacee_len = strlen(replacee);
	occurrence = StringFindAllOccurrencesCStringTarget(str, replacee);
	if (occurrence.count == 0) return *str; // return the same string, no matches were found
	ptrdiff_t delta = (ptrdiff_t)replacer_len - (ptrdiff_t)replacee_len; //  
	str_copy.length = (size_t)((ptrdiff_t)str->length + (ptrdiff_t)occurrence.count * delta);
	str_copy.base = (char*)malloc(str_copy.length);
	// Copying mechanism
	for (size_t copyindx = 0, occurindx = 0, srcindx = 0; copyindx < str_copy.length && srcindx < str->length; copyindx++, srcindx++)
	{
		if (occurindx < occurrence.count && srcindx == occurrence.items[occurindx])
		{
			memcpy(str_copy.base + copyindx, replacer, replacer_len);
			copyindx += replacer_len - 1; // Since it is replaced, increase the copying index by the size of replacer.
			srcindx += replacee_len - 1; // Since the replacee is in the original string, increase the srcindx by replacee.
			occurindx++;
			continue;
			/*The indicies lose their sync the moment we replace */
		}
		str_copy.base[copyindx] = str->base[srcindx];
	}
	return str_copy;
}