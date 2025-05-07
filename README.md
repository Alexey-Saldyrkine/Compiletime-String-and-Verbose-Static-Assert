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
Returns a basic string view of the string.<br>
Example:<br>
constexpr std::basic_string_view strView = decltype("hello world"_compStr)::to_basic_string_view; // strView will contain "hello world"

### sv
const std::basic_string_view = compString::sv<br>
Same as to_basic_string_view.<br>

## Modifiers
__NOTE:__ these functions do __not__ change the compString that calls them, but return a new compString that __is__ modified.<br>

### append
compString = compString::append\<compString str><br>
Returns a compString with str appended to the end of the compString.<br>
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

### pop_back
compString = compString::pop_back<br>
Return a compString with all but the last character.<br>
Example:<br>
using retStr = decltype("hello my world"_compStr)::pop_back; // retStr contains "hello worl"<br>

## Search

### find
size_t = compString::find\<compString str, size_t pos = 0><br>
Returns the index of the first occurrence of str in the compString, after pos.<br>
Pos defaults to 0.<br>
If no occurrence is found, -1ull will be returned.<br>
Example:<br>
constexpr size_t i =  decltype("hello my world"_compStr)::find\<decltype("my"_compStr)>; // i = 6

### rfind 
size_t = compString::rfind\<compString str, size_t pos = compString::size><br>
Returns the index of the last occurrence of str in the compString, before pos.<br>
Pos defaults to compString::size.<br>
If no occurrence is found, -1ull will be returned.<br>
Example:<br>
constexpr size_t i =  decltype("my, oh my"_compStr)::rfind\<decltype("my"_compStr)>; // i = 7

### find_first_of
size_t = compString::find_first_of\<compString str, size_t pos = 0>
Returns the index of the first occurrence ,of any character of str, in compString, after pos.<br>
Pos defaults to 0.<br>
If no occurrence is found, -1ull will be returned.<br>
Example:<br>
constexpr size_t i = decltype("hello my world"_compStr)::find_first_of\<decltype("mylw"_compStr)>; // i = 2

### find_first_not_of
size_t = compString::find_first_not_of\<compString str, size_t pos = 0>
Returns the index of the first occurrence, of any character not in str, in compString, after pos.<br>
Pos defaults to 0.<br>
If no occurrence is found, -1ull will be returned.<br>
Example:<br>
constexpr size_t i = decltype("hello my world"_compStr)::find_first_not_of\<decltype("ole h"_compStr)>; // i = 6

### find_last_of
size_t = compString::find_last_of\<compString str, size_t pos = compString::size>
Returns the index of the last occurrence, of any character of str, in compString, before pos.<br>
Pos defaults to compString::size.<br>
If no occurrence is found, -1ull will be returned.<br>
Example:<br>
constexpr size_t i = decltype("hello my world"_compStr)::find_last_of\<decltype("mylw"_compStr)>; // i = 12

### find_last_not_of
size_t = compString::find_last_not_of\<compString str, size_t pos = compString::size><br>
Returns the index of the last occurrence, of any character not in str, in compString, before pos.<br>
Pos defaults to compString::size.<br>
If no occurrence is found, -1ull will be returned.<br>
Example:<br>
constexpr size_t i = decltype("hello my world"_compStr)::find_last_not_of\<decltype("mylwo rd"_compStr)>; // i = 1

## Operations

### equal
bool = compString::equal\<compString str><br>
Checks wether compString is equal to str<br>
Example:<br>
constexpr bool b = decltype("hello"_compStr)::equal\<decltype("hello"_compStr)>; // b = true<br>
constexpr bool b = decltype("hello"_compStr)::equal\<decltype("hello "_compStr)>; // b = false

### lesser
bool = compString::lesser\<compString str><br>
Checks wether compString is lesser than str.<br>
Can use lesserEq for <=.<br>
Example:<br>
constexpr bool b = decltype("hello"_compStr)::lesser\<decltype("zzz"_compStr)>; // b = false<br>
constexpr bool b = decltype("hello"_compStr)::lesser\<decltype("aaaaaa"_compStr)>; // b = true

### greater
bool = compString::greater\<compString str><br>
Checks wether compString is greater than str.<br>
Can use greaterEq for >=.<br>
Example:<br>
constexpr bool b = decltype("hello"_compStr)::greater\<decltype("zzzzzz"_compStr)>; // b = false<br>
constexpr bool b = decltype("hello"_compStr)::greater\<decltype("aaa"_compStr)>; // b = true

### compare
int = compString::compare\<compString str><br>
Compares compString and str in lexicographical order from left to right.<br>
If compString is greater than str, then it returns 1.<br>
If compString is lesser than str, then it returns -1.<br>
If compString is equal to str, then it returns 0.<br>
Example:<br>
constexpr int i = decltype("hello my world"_compStr)::compare\<decltype("hello our world"_compStr)>;// i = -1

int = compString::compare\<compString str, size_t pos1, size_t count1, size_t pos2, size_t count2><br>
Compares substring of compString in range \[pos1,pos1+count1] and substring of str in range \[pos2,pos2+count2].<br>

### starts_with
bool = compString::starts_with\<compString str>
Checks wether compString starts with the string str.<br>
Example:<br>
constexpr bool b = decltype("hello my world"_compStr)::starts_with\<decltype("hello"_compStr)>; // b = true

### ends_with
bool = compString::ends_with\<compString str>
Checks wether compString end with the string str.<br>
Example:<br>
constexpr bool b = decltype("hello my world"_compStr)::starts_ends\<decltype("word"_compStr)>; // b = false

### substr
compString = compString::substr\<size_t pos, size_t count><br>
Returns a compString that is the substr of compString in range \[pos,min(pos+count,compString::size)].<br>
Example:<br>
using retStr = decltype("hello my world"_compStr)::substr\<3,4>; // retStr contains "lo m"

### substrLR
compString = compString::substr\<size_t pos1, size_t pos2><br>
Returns a compString that is the substr of compString in range \[pos1,min(pos2,compString::size)].<br>
Pos1 must be lesser than pos2.<br>
Example:<br>
using retStr = decltype("hello my world"_compStr)::substr\<3,8>; // retStr contains "lo my"

### erase_if
compString = compString::erase_If\<typename pred>
pred is the predicate that determines wether, a character is erased.<br>
pred must be default constructable and must have an operator().<br>
A lambda can be used.<br>
The operator() must have a function signature of one of the following:
- bool(char)
- bool(size_t)
- bool(char,size_t)
- bool(size_t,char)
  
Where the paramater char is the current character being considered.<br>
Where the paramater size_t is the index of the current character being considered.<br>
Where bool determines wether the character is deleted.<br>
Example:<br>
using pred= decltype( \[](char c)->bool{<br>
  return c =='l';<br>
});<br>
//pred will delete all 'l' in the string
using retStr = decltype("hello world"_compStr)::erase_if\<pred>; // retStr contains "heo word"

### replace_if
compString = compString::replace_if\<typename funcT>
funcT is a functor type that will return the value that the current character will be replaced with.<br>
funcT must be default constructable and must have an operator().<br>
A lambda can be used.<br>
The operator() must have a function signature of one of the following:
- char(char)
- char(size_t)
- char(char,size_t)
- char(size_t,char)
  
Where the paramater char is the current character being considered.<br>
Where the paramater size_t is the index of the current character being considered.<br>
Where return char determines with what character the current character is replaced with.<br>
Example:<br>
 using replaceFunc = decltype( \[](char c)->char{<br>
 if(c == 'l')<br>
   return 'z';<br>
  else<br>
    return c;<br>
  });<br>
  // replaceFunc will replace all 'l' with 'z'<br>
  using retStr = decltype("hello world"_compStr)::replace_with\<repalceFunc>; // resStr will contain "hezzo worzd"

