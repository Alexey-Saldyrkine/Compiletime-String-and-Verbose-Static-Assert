### Compiletime String

The type compsString is the type thats acts as a string,<br>
located in the namespace 'CompStringNS'.<br>

A compString is decleard using the type of the user-defiend literal _compStr.<br>
Example:<br>
using str = decltype("hello world"_compStr); // 'str' is  a compString type containing the string "hello world".<br>

### member types / functions
The member types of compString act as member functions 'returning' a type or value.
CompString has most functions that [std::string](https://en.cppreference.com/w/cpp/string/basic_string) contains.<br>
As aliases are not mutable any 'function', that would modify std::string, instead return the modified compString as a new type.<br>

## Size
size_t = compString::size<br>
Returns the length of the string compString contains. Type is size_t.<br>
Example:<br>
constexpr size_t len = decltype("hello world"_compStr)::size; // len = 11<br>

## Element access

### at 
char = compString::at\<size_t pos><br>
Returns the character at the postion pos of the string.<br>
Example:<br>
constexpr char c = decltype("hello world"_compStr)::at<6>; // c = 'w'<br>

### front
char = compString::front<br>
Returns the first character of the string.<br>
Equivalent to compString::at\<0>.<br>
Example:<br>
constexpr char c = decltype("hello world"_compStr)::front; // c = 'h'<br>

### back
char = compString::back\<br>
Returns the last character in the string.<br>
Equivalent to compString::at\<compString::size-1>.<br>
Example:<br>
constexpr char c = decltype("hello world"_compStr)::back; // c = 'd'<br>

### c_str
const char* = compString::c_str
Return a const char* to a null terminated array of characters of length compString::size+1, that represents the string.<br>
Example:<br>
const char* strP =  decltype("hello world"_compStr)::c_str;<br>
std::string str(strP); // str will be constructed and contain "hello world"<br>

### data
const char* = compString::data<br>
Same as c_str.<br>

### to_basic_string_view
const std::basic_string_view = compString::to_basic_string_view<br>
Returns a basic string view of the string
Example:<br>
constexpr std::basic_string_view strView = decltype("hello world"_compStr)::to_basic_string_view;<br>

### sv
const std::basic_string_view = compString::sv<br>
Same as to_basic_string_view.<br>

## Modifiers
__NOTE:__ these functions do __not__ change the compString that calls them, but return a new compString that __is__ modified.<br>

### append
compString = compString::append\<compString str><br>
Returns a compString with str appended to the end of the compString<br>
Example:<br>
using retStr = decltype("AA"_compStr)::append\<decltype("BB"_compStr)>; // retStr contains "AABB"

### prepend
compString = compString::prepend\<compString str><br>
Returns a compString with str prepended to the begginning of the compString.<br>
Example:<br>
using retStr = decltype("AA"_compStr)::prepend\<decltype("BB"_compStr)>; // retStr contains "BBAA"

### clear
compString = compString::clear<br>
Returns an empty compString.<br>
Equivalent to decltype(""_compStr).<br>
Example:<br>
using retStr = decltype("hello world"_compStr)::clear; // retStr is an empty compString

#### insert
compString = compString::insert\<size_t pos, compString str><br>
Returns a compString with the compString str inserted after the character at pos.<br>
Example:<br>
using retStr =  decltype("hello world"_compStr)::insert<5,decltype("my "_compStr)>; // retStr contains "hello my wolrd"<br>

### erase
compString = compString::erase\<size_t pos, size_t count><br>
Returns a compString that contains all characters, except the characters in the range \[pos,min(size,pos+count)].<br>
Example:<br>
using retStr = decltype("hello my world"_compStr)::erase<5,3>; // retStr contains "hello world"<br>

### push_back
compString = compString::push_back\<char chr><br>
Returns a compString with char appended to the end.<br>
Example:<br>
using retStr = decltype("hello my world"_compStr)::push_back<'s'>; // restStr contains "hello worlds"<br>

## pop_back
compString = compString::pop_back<br>
Return a compString with all but the last character.<br>
Example:<br>
using retStr = decltype("hello my world"_compStr)::pop_back; // retStr contains "hello worl"<br>

## Search

### find
size_t = compString::find\<compString str><br>
Returns the index of the first occurrence of str in the compString.<br>
Example:<br>
constexpr size_t i =  decltype("hello my world"_compStr)::find\<decltype("my"_compStr)>; // i = 6

### rfind 
size_t = compString::rfind\<compString str><br>
Returns the index of the last occurrence of str in the compString.<br>
Example:<br>
constexpr size_t i =  decltype("my, oh my"_compStr)::rfind\<decltype("my"_compStr)>; // i = 7

### find_first_of
size_t = compString::find_first_of\<compString str>
Returns the index of the first occurrence ,of any character of str, in compString.<br>
Example:<br>
constexpr size_t i = decltype("hello my world"_compStr)::find_first_of\<decltype("mylw"_compStr)>; // i = 2

### find_first_not_of
size_t = compString::find_first_not_of\<compString str>
Returns the index of the first occurrence, of any character not in str, in compString.<br>
Example:<br>
constexpr size_t i = decltype("hello my world"_compStr)::find_first_not_of\<decltype("ole h"_compStr)>; // i = 6

### find_last_of
size_t = compString::find_last_of\<compString str>
Returns the index of the last occurrence ,of any character of str, in compString.<br>
Example:<br>
constexpr size_t i = decltype("hello my world"_compStr)::find_last_of\<decltype("mylw"_compStr)>; // i = 9
