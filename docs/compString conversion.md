# conversion to compString
For convinces, there are several types that convert values (such as: 19, "2ab3",'l',...) and types (such as: bool, int, double,...) to a compString containing a string representation of them.<br>

They are:
- valueToCompString
- typeToCompString
- templatedTypeToCompString

These types take the value or type that will be converted as a template parameter and have a member type named 'type' that is a compString containing the result of conversion.<Br>

These types are located in the namespace compStringNS::compStringConvNS

## value to compString
Currently value to compString supports conversion of values that are one of the types:<br>
- bool
- char
- integrals
- null terminated char arrays

valueToCompString can be manually extended to convert other types.<br>

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

### extending conversion to other types 
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

### If no conversion exists
If no conversion for a value of type T exists, the compString "[no value to compString conversion for the type 'T']" will be returned.<br>
Where T will be the result of typeToCompString\<T>.<br>

## type To compString 

