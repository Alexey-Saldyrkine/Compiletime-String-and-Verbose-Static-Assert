# conversion to compString
For convinces, there are two types that convert values (such as: 19, "2ab3",'l',...) and types (such as: bool, int, double,...) to a compString containing a string representation of them.<br>

They are:
- valueToCompString
- typeToCompString

These types take the value or type that will be converted as a template parameter and have a member type named 'type' that is a compString containing the result of conversion.<Br>

These types are located in the namespace compStringNS::compStringConvNS

## value to compString
Currently value to compString supports conversion of values that are one of the types:<br>
- bool
- char
- integrals
- null terminated char arrays

valueToCompString can be manually extended to convert values of other types.<br>

### If no conversion exists
If no conversion for a value of type T exists, the compString "[no value to compString conversion for the type 'T']" will be returned.<br>
Where T will be the result of typeToCompString\<T>.<br>

### bool
A value of type bool (true, false) will be converted to a compString containing "true" and "false" respectively.<br>
Example:<br>
```cpp
using stra = typename valueToCompString<true>::type; // stra contains "true"
using strb = typename valueToCompString<false>::type; // strb contains "false"
```

### char 
A value of type char ('a','h',...) will be converted to their respective symbol.<br>
Example:<br>
```cpp
using strh = typename valueToCompString<'h'>::type; // strh contains "h"
Using strU = typename valueToCompString<'U'>::type; // strU contains "U"
``` 

### integrals 
Values of integral types (decided by std::is_integral) will be converted to a string representation of the number.<br>
If the value is negative (<0), then the character symbol '-' will be at the begging of the result.<br>
Example:<br>
```cpp
using str1 = typename valueToCompString<17432>::type; // str1 contains "17432"
using str2 = typename valueToCompString<-99432>:::type; // str2 contains "-99432"
```

### null terminated character array
Giving a value  of const char* as the template parameter valueToCompString, will be interpreted as a null terminated char array.<br>
Example:<br>
```cpp
static constexpr char ptr[] = "elvis curl";
constexpr auto sptr = valueToCompString<ptr>::type::sv;
static_assert(sptr == "elvis curl"sv); // no error occurs 
```

### char arrays with a given length
For converting a char array with a given length you can use the type charArrayToType\<const char* adrs, size_t len>.<br>
Example:<br>
```cpp
static constexpr char ptr[] = "elvis curl";
constexpr auto sptr  = charArrayToType<ptr,5>::type::sv;
static_assert(sptr == "elvis"sv); // no error occurs
```

### extending value conversion to other types 
To extend valueToCompString conversion for a value of type T, you will need to create a template specialization of valueToCompStringInter within the namespace compStringNS::compStringConvNS.<br>
Example:<br>
```cpp
using T = sometype; // T can be any type. T can be removed and replaced by sometype
namespace compStringNS{
namespace compStringConvNS{ //tempalte specializations need to be in the same namespace as the base tempalte 
	template<T value> // only one parameter: a value of type T named 'val'
	struct valueToCompStringInter<T,val>{ // here we specialize with two parameters: first is the type T, second is a value of type T
		using type = typename someTypeToCompString_Impl<val>::type; // here will be the implementation of the conversion
		// member type 'type' must be present and be of a compString type
	};
}
}
```

## type To compString 

Conversion of a type to a compStrirng is done by using the helper type typeToCompString.<br>
typeToCompString takes a  type as a template parameter and has a member type named 'type' which is the string representation of the type.<br>

typeToCompString can be manually extended to convert other types, such as, user-defined types.<br>

typeToCompString can accept any type.<br>
Templated types will have their own section, separate from the rest.<br>

## non-templated types to compString

This includes:
- fundamental types
- user-defined types
- const volatile qualified types
- compound types:
	- references
	- pointer types
	- pointer-to-member types
	- function types
	- array Types

### fundamental types
All fundamental types ([as described by this page](https://en.cppreference.com/w/cpp/language/types.html), except for std::nullptr_t) are supported by default.<br>
Specifically, the following types are supported:
```cpp
void, bool, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, 
long int, unsigned long int, long long int, unsigned long long int, float, double, long double 
```
Examples:
```cpp

using voidStr = typename typeToCompString<void>::type;
static_assert(voidStr::sv == "void"sv);

using ulliStr = typename typeToCompString<unsigned long long int>::type;
static_assert(ulliStr::sv == "unsigned long long int"sv);

using doubleStr = typename typeToCompString<double>::type;
static_assert(doubleStr::sv == "double"sv);
```
### user-defined types
user-defined types can be converted to a compString, if it has been added to the list of possible conversion types.<br>
User-defined types (and the fundamental types, by extension) use SFINAE and template specialization to choose which compString represents a type.<br>
In the namespace ::compStringNS::compStringConvNS::typeToCompStringDefinitions there is the type typeToCompStringInter<typename T>.<br>
To add a type to the list of convertible types you will need to create a template specialization of typeToCompStringInter with the wanted type.<br>
Example:
```cpp

struct myType{}; // the type we will add to the convertible type list

namespace compStringNS::compStringConvNS::typeToStringDefinitions{
	template<> 
	struct typeToCompStringInter<myType>{ // the specialization of typeToCompStringInter
		using type = decltype("myType"_compStr); // the compString that will be returned 	
	};
}
```
Alternatively you can use the macro createTypeDefinition within the namespace to automatically define the specialization and compString:
```cpp
struct myType{}; // the type we will add to the convertible type list

namespace compStringNS::compStringConvNS::typeToStringDefinitions{
	createTypeDefinition(myType); // automatically create the template specialization and compString "mtType", adding it to the convertible types	
}
```

__NOTE:__ you should __NOT__ add cv qualified or compound(&,&&,*) types to the convertible types, only the one basic, non compound, non cv qualified type.<br>
Example:<br>
You should only add myType, not any of the following:<br>
```cpp
myType&, const myType, myType*, const volatile myType***** const, ... so on and so fourth
``` 
If you do add a cv-qualified and or compound type, it will most likely never be used, as in the process of converting a type to compString it is gradually striped of cv-qualifiers and compounds.

Templated types can be added, but will only trigger if two conditions are met:
For the templated type T and the type that is being converted U
- std::is_same_v<T,U> evaluates to true
- the template of T has not been added to the convertible template types

### if no conversion exists
If no conversion for a type exists, the compString "[no name given to type]" will be returned.<br>
__NOTE:__ this only applies to the base type, if a compound and or cv-qualified type is given it will return a compString containing the cv-qualifiers and or pointers and references.<br>
Example:
```cpp
struct nonAddedT{}; // this type will mot be added to the convertible types

using U = typename typeToCompString<nonAddedT>::type;
static_assert(U::sv == "[no name given to type]");
using T = typename typeToCompString<const nonAddedT&>::type;
static_assert(T::sv == "const [no name given to type]&");
using H = typeToCompString<volatile nonAddedT* const* volatile*&&>::type;
static_assert(H::sv == "volatile [no name given to type]* const* volatile*&&");

```


### const volatile qualifiers 	

During the conversion process of the type T, if T can be deduced to const U, volatile U, or const volatile U, then the type U is converted to a compString.<br>
Depending on if U is a pointer or not, the appropriate compString "const", "volatile", "const volatile" will be prepended or appended.<br>
__NOTE__: a type given as 'volatile const U' will be converted to "const volatile U" __not__ "volatile const U"<br>
Const will always come before volatile.<br>

Examples:<br>
```cpp

using T0 = typename typeToCompString<const int>::type; // will be compString containing "const int"

using T1 = typename typeToCompString<int* volatile>::type; // will be compString containing "int* volatile"

using T2 = typename typeToCompString<const volatile int* const* volatile>::type; // will be compString containing "const volatile int* const* volatile"

using T3 = typename typeToCompString<volatile const int>::type; // will be compString containing "const volatile int"

```

## compound types

### lvalue and rvalue references 

During the conversion processes of the type T, if T can be deduced to U& or U&&, then U is converted to a compString, and the appropriate string "&" or "&&" is appended.<br>

Examples:

```cpp

Using T0 = typename typeToCompString<int&>; // will be compString containing "int&"

Using T1 = typename typeToCompString<int &&>; // will be compString containing "int&&"

Using T2 = typename typeToCompString<int* const&>; // will be compString containing "int* const&"

```

### pointer types

During the conversion process of the type T, if the base type of T is not a function type and can be deduced to U*, then U is converted to a compString and "*" will be appended.<br>

Examples:
```cpp

using T0 = typename typeToCompString<int*>; // will be compString containing "int*"

Using T1 = typename typeToCompString<int**>; // will be compString containing "int**"

``` 

### function pointer types

During the conversion process of the type T, if the base type of T is a function type F and if during the conversion process there are one or more pointer types identified, 
then the function type F is converted to a compString FuncStr and all the pointers and their qualifiers are converted to a compString PtrStr. The compString "(PtrStr)" will be inserted between the return type and arguments of FuncStr.<br>

Example:
```cpp

using T0 = typename typeToCompString<int(*)(int)>; // will be compString containing "int(*)(int)"

using T1 = typename typeToCompString<int(***)(int)>; // will be compString containing "int(***)(int)"

```

### pointer-to-member types

During the conversion process of the type T, if T be deduced to H U::*, U is converted into the compString Hstr and then the steps for pointer types follow with PtrStr being "U::*".<br>

Example:

```cpp

struct myType{
	int memC;
	int memF(int);
};

namespace compStringNS::compStringConvNS::typeToStringDefinitions{
	createTypeDefinition(myType); //  adding it to the convertible types	
}

using T0 = typename typeToCompString<&myType::memC>; // will be compString containing "int myType::*"

using T1 = typename typeToCompString<&myType::memF>; // will be compString containing "int(myType::*)(int)"


```

### function types
During the conversion process of the type T, if T deduced to a function type R(Args...) including variations of cv-qualifiers, references, noexcept and variadic functions.<br>
R and each Args types are converted to a compString and the compString "R(Args0, Arg1, ..., ArgsN)REF CV NOEXCEPT" is returned;

Examples:
```cpp

using T0 = typename typeToCompString<int(int,bool)&>; // will be compString containing "int(int, bool)&"

using T1 = typename typeToCompString<int(int,bool...) const>; // will be compString containing "int(int, bool...) const"

using T2 = typename typeToCompString<int(int,bool,double...) const volatile&& noexcept>; // will be compString containing "int(int, bool, double...) const volatile&& noexcept"

```

### Array types
During the conversion process of the type T, if T deduced to U[] or U[N], then U and N are converted to compString and the compString "U[]" or "U[N]" is returned.<br>

Examples:
```cpp

using T0 = typename typeToCompString<bool[]>; // will be compString containing "bool[]"

using T1 = typename typeToCompString<int[123]>; // will be compString containing "int[123]"


```

## templated types to compString
Note: template template types are not currently supported, their is a mechanism to get the name of a template.

Due to the lack of a universal template parameter, in order to accommodate all possible templates the addition of a template to the possible conversion types is a little different than non-template types.<br>

Within the namespace ::compStringNS::compStringConvNS::templatedTypeDefinitions there is a type templatedTypeToCompString<typename T>.<br>
There are also two helper types mp_list, valueAsType, templateTemplateType.<br>
mp_list is a type list that will contain all the parameters of the template on order.<br>
valueAsType converts a value to type, it must be used to convert non-type template parameters into types. If a value is not wrapped into this type, it will not be recognized as a value.<br>
templateTemplateType is a wrapper type for a template. If a template is not wrapped by this type it will not be recognized as template template parameter.<br>
To add a template to the convertible template types, you will need to create a specialization of templatedTypeToCompString.<br>
Each specialization of templatedTypeToCompString must have two member types: typeName and typeList, where typeName is a compString representing the template, and typeList is a mp_list of all template parameters in the form of types.<br>

Lets say we have a template T<args...>, where args can be type template parameter, non-type template parameter or template template parameter.<br>
We will have the specialization of templatedTypeToCompString:
```cpp
template<args...>
Struct T{}; // template type that will be added to the convertible types

namespace compStringNS::compStringConvNS::templatedTypeDefinitions{ // specialization must be in the correct namespace

template<args...>
struct templatedTypeToCompString< T<args...> >{ // specialization of templatedTypeToCompString with T
	using typeName = decltype("T"_compStr); // the name that will be represent T
	using typeList = mp_list<args...>; // the parameters of T in a mp_list, all non-type and template template parameters will need to be converted to types.
};
}
```

Example:
```cpp

template<template<typename...> typename TempT, typename A, int I, typename... Ts>
struct myTemplatedType{}; // type to be added

template<typename...>
struct templateTemplateParameterExampleType{}; // type to show how to pass template template into the mp_list 

namespace compStringNS::compStringConvNS::templatedTypeDefinitions{ // specialization must be in the correct namespace

// addition of templateTemplateParameterExampleType to convertible template types
template<typename... Ts>
struct templatedTypeToCompString<templateTemplateParameterExampleType<Ts...>>{
	using typeName = decltype("TTP Example Type"_compStr);
	using typeList = mp_list<Ts...>;
};

template<template<typename...> typename TempT, typename A, int I, typename... Ts> // same template parameters as myTemplatedType
struct templatedTypeToCompString< myTemplatedType<TempT,A,I,Ts...> >{ // specialization of templatedTypeToCompStrin
	using typeName = decltype("myTemplatedType"_compStr);
	using wrappedTT = templateTemplateType< TempT<void> >; // here we wrap the template template parameter with arbirtary arguments, as we need TempT as a type
	using typeList = mp_list<wrappedTT, A, valueAsType<I>, Ts...>; // note how we took the non-type I and converted it to a type using valueAsType
};
}

using U = typename typeToCompString<templateTemplateParameterExampleType<int,void,double>>::type;

static_assert(U::sv == "TTP Example Type<int, void, double>"sv,U::sv);

using T = typename typeToCompString< myTemplatedType<templateTemplateParameterExampleType,char,5,int,bool,char> >::type;
static_assert(T::sv == "myTemplatedType<TTP Example Type, char, 5, int, bool, char>"sv,T::sv);

```

__NOTE:__ if the member type typeList is equal to void, the specialization of the templated type will not be acknowledged and the templated type will be interpreted as a non-template type go check if ::compStringNS::compStringConvNS::typeToCompStringDefinitions::typeToCompStringInter has the specific templated type as a specialization.<br>






