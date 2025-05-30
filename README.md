# Table of Contents
- [Compile time string](#Compile-time-String)
	- [Member functions](#Member-functions)
	- [value to compString](#conversion-of-value-and-types-to-compString)
	- [compString to value](#compString-to-value)
- [Verbose static assert](#Verbose-static-assert)
- [Requirements](#Requirements) 
# Compile time String

compString is a type that mimics std::string.<br>
compString has a std::integerSequence as a template parameter to store the characters of the string.<br>
As such, when passing a compString as a template parameter, all the information about the string is passed with the compString, independent of any other compString type or other identifiers.<br>
Because compString exists entirely as a type and that types aren't mutable, any string function, that would modify its self, instead returns a new compString type containing the changes.<br>
It can be thought in a zero side effect, functional paradigm.<br>


## Member functions
Instead of using member functions to represent the functions of std::string, compString uses member types and values.<br>
If a function of std::string has parameters, then a templated type or value is used, where the template parameters are used instead of function parameters.<br>
Example:<br>

In std::string the function push_back will be called as such:<br>
```cpp
std::string str = "hello world";
str.push_back('!');
```
Here str will be modified to contain "hello world!".<br>

In compString the 'function' will be 'called' using the templated member type push_back:<br>
```cpp
Using cmpStr = decltype("hello world"_compStr);
Using newCmpStr = cmpStr::push_back<'!'>;
```
Here newCmpStr will contain the modified string "hello world!", while cmpStr will remain unchanged.<br>

compString has most of the functions of std::string.<br>
The full list can be found at docs/compString.md<br>
Notably functions that work with runtime memory allocation/deallocation (reserve, capacity, ...) are not present, as compString exists within compile time.<br>

## conversion of value and types to compString
For the conversion of types to compString there is a helper type typeToCompStringInter\<typename T>.<br>
If the type T can be converted to a string representation by typeToCompStringInter, than the member type 'type' will be it, 
otherwise it will be a compString of "[no name given to type]".<br>
For the conversion of values to compString there is a helper type valueToCompString\<valueType value>.<br>
If the value can be converted to a string representation by valueToCompString, than the member type 'type' will be it,
otherwise it will be a compString of "[no name given to value]".<br>
For details see docs/compStringConv.md<br>


## compString to value
If you would like to convert a compString type to a value, you could get a constexpr std::string_view using the member values to_basic_string_view or the shorthand sv.<br>
Then the std::string_view value can be used in compile time or runtime.<br>


# Verbose static assert
With c++26 static assert can use a [constant-expression(3)](https://en.cppreference.com/w/cpp/language/static_assert) as the error message.<br>
A constexpr std::string_view can be used as a constant-expression.<br>
Thus we can generate a compString based on template parameters in a dynamic manner, giving the user (in this case another programmer) a more detailed error message, specifying the exact reason(s) for the error.<br>
Verbose static assert is a wrapper around static assert that accepts an expression and a error message template, as template parameters.<br>
If the expression evaluates to false, a message will be generated based of the message template using compString and output the message with the compilers error message.<br>

An example of this is a metafunction that checks if all types of a variadic template parameter pack are unique:<br>
The type are_all_different<Ts...> takes a template parameter pack Ts. If all types in Ts are unique, then the member value bool 'value' == true, otherwise 'value' == false.<br>
The type are_all_differant_VSA_message is a templated type that will check for any duplicate types int it.<br>
If two types are the same, then the type and 0-indexed positions of the found types will be added to the error message.<br>

So the code:<br>
```cpp
using y = are_all_differant<int,long int,int,char,float,double,char,double>;
verbose_static_assert<y,are_all_differant_VSA_message>{};
```

Will out put a compiler error:<br>
```cpp
...
error: static assertion failed:
Same types found:
found same type int at pos 0 and 2,
found same type char at pos 3 and 6, 
found same type double at pos 5 and 7, 
found same type double at pos 5 and 8, 
...
```

This dynamic message is more helpful and useful than a static "found at least two equal types" message.<br>

For details see docs/verbose static assert.md<br>

# Requirements 
compString requires c++20 or higher.<br>
Verbose static assert requires c++26 for constant-expression static assert message.<br> 



  