### Compiletime String

compString is a type that mimics std::string.<br>
compString has a std::integerSequence as a template parameter to store the characters of the string.<br>
As such, when passing a compString as a template parameter, all the information about the string is passed with the compString, independent of any other compString type or other identifiers.<br>
Because compString exists entirely as a type and that types aren't mutable, any string function, that would modify its self, instead returns a new compString type containing the changes.<br>
It can be thought in a zero side effect, functional paradigm.<br>


## member functions
Instead of using member functions to represent the functions of std::string, compString uses member types and values.<br>
If a function of std::string has parameters, then a templated type or value is used, where the template parameters are used instead of function parameters.<br>
Example:<br>
In std::string str the function push_back will be called as such:<br>
str.push_back('!');<br>
Here str will be modified.
In compString cmpStr the 'function' will be 'called' using the templated member type push_back:<br>
Using newCmpStr = cmpStr::push_back\<'!'>;<br>
Here newCmpStr will contain the modified string, while cmpStr will remain unchanged.<br>

compString has most of the functions of std::string.<br>
The full list can be found at docs/compString.md<br>
Notably functions that work with runtime memory allocation/deallocation (reserve, capacity, ...) are not present, as compString exists within compile time.<br>

## getting a string as a value
If you would like to convert a compString type to a value, you could get a std::string_view using the member values to_basic_string_view or the shorthand sv.<br>
Then the std::string_view value can be used to convert to std::string or any other type.<br>
The std::string_view value can be used during compile time and run time.<br>


### Verbose static assert
With c++26 static assert can use a [constant-expression](https://en.cppreference.com/w/cpp/language/static_assert)(3) as the error message.<br>
A constexpr std::string_view can be used as a constant-expression.<br>
Thus we can generate a compString based on template parameters in a dynamic manner, giving the user (in this case another programmer) a more detailed error message, specifying the exact reason(s) for the error.<br>
Verbose static assert is a wrapper around static assert that accepts an expression and a error message template, as template parameters.<br>
If the expression evaluates to false, a message will be generated based of the message template using compString and output the message with the compilers error message.<br>

An example of this is a metafunction that checks if all types of a variadic template parameter pack are unique:<br>
...

   


  