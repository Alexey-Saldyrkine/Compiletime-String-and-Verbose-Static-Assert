#include "compString_conversion.hpp"


namespace{
    using namespace std::literals;
    using namespace compStringNS;
    using namespace compStringNS::compStringConvNS;
    template<auto V>
    constexpr auto vToSV = valueToCompString<V>::type::sv;
    struct compStringConvTestsValue{
        #if __cpp_static_assert >= 202306L
        #define vToSvTestI(x) static_assert(vToSV<x> == #x""sv,vToSV<x>);
        #define vToSvTest(x,str) static_assert(vToSV<x> == #str""sv,vToSV<x>);
        #else
        #define vToSvTestI(x) static_assert(vToSV<x> == #x""sv);
        #define vToSvTest(x,str) static_assert(vToSV<x> == #str""sv);
        #endif
        //integer to compString
        static_assert(vToSV<0> == "0"sv);
        static_assert(vToSV<'a'> == "a"sv);
        static_assert(vToSV<'z'> == "z"sv);
        static_assert(vToSV<'\\'> == "\\"sv);
        static_assert(vToSV<'&'> == "&"sv);
        static_assert(vToSV<true> == "true"sv);
        static_assert(vToSV<false> == "false"sv);

        vToSvTestI(1);
        vToSvTestI(-1);
        vToSvTestI(22);
        vToSvTestI(-123);
        vToSvTestI(321);
        vToSvTestI(-63);
        vToSvTestI(1234);
        vToSvTestI(-6345);
        vToSvTestI(12345);
        vToSvTestI(-73529);
        vToSvTestI(234623423622346);
        vToSvTestI(-234623423622346);
        vToSvTest(-1ull,18446744073709551615);
        vToSvTest(1ull,1);
        vToSvTest(-1u,   4294967295);
        vToSvTest(-2353u,4294964943)

        #undef vToSvTestI
        #undef vToSvTest
        // const char*

        #define constCharPTests(n,x) static constexpr const char TTP_##n []= x; static_assert( vToSV<TTP_##n> == x""sv);

        constCharPTests(a,"hello world, abra, snore, galaxy world");
        constCharPTests(b,"23465134rsadf2gqraskfiq432t wmetrntq2u3nr v  m3ir 23trm w4it 20it imweiomt23mr i");

        #undef constCharPTests
    };

    struct compStringConvTypes{
        template<typename T>
        static constexpr auto tToSv = typeToCompString<T>::type::sv;
        #if __cpp_static_assert >= 202306L
        #define testType(x) static_assert(tToSv<x> == #x""sv,tToSv<x>);
        #else
        #define testType(x) static_assert(tToSv<x> == #x""sv);
        #endif
        testType(void);
        testType(char);
        testType(signed char);
        testType(unsigned char);
        testType(char&);
        testType(char&&);
        testType(const char&);
        testType(volatile char);
        testType(const volatile char&&);
        testType(char***);
        testType(char*&);
        testType(const char* const volatile&);
        testType(char*);
        testType(char* const);
        testType(char* volatile);
        testType(char* volatile* const* volatile* const volatile*);
        testType(char* const volatile);
        testType(const char* const);
        testType(const volatile char* const volatile);
        testType(unsigned char);
        testType(signed char);
        testType(int);
        testType(const int);
        testType(const volatile int);
        testType(volatile int);
        testType(char);
        testType(const char);
        testType(volatile char);
        testType(const volatile char);
        testType(long int);
        testType(long long int);
        testType(const volatile long long int);
        testType(float);
        testType(const volatile double);
        testType(bool());
        testType(bool(char));
        testType(bool(char, int));
        testType(int(char, int, double));
        testType(int(char, int, double) const);
        testType(int(char, int, double) const volatile);
        testType(int(char, int, double)&);
        testType(int(char, int, double)&&);
        testType(int(char, int, double) volatile&&);
        testType(int(*)(char, int, double));
        testType(const int(*)(char, int, double));
        testType(int(char, int, double) const&);
        testType(int(char, int, double) volatile&&);
        testType(const int(*&)(char, int, double));
        testType(const int(*&&)(char, int, double));
        testType(const double&(const char*, float&, bool));
        testType(const double&(const char*, float&, bool) const volatile&&);
        testType(const double&(*&&)(const char*, float&, bool));
        testType(int(* const&&)(char, int, double));
        testType(int(* const volatile&&)(char, int, double));
        testType(int(********)(char, int, double));
        testType(bool(* const&)(bool(*)(int), bool(*)(int)));
        testType(int(* const volatile* const* const&&)(char, int, double));
        testType(int(*)(char, int, double,...));
        testType(bool[]);
        testType(int[123]);
        testType(int** const volatile* const****);
        testType(int(int, bool, double) const volatile&&);
        testType(int(int, bool, double) noexcept);
        testType(int(int, bool, double) const noexcept);
        testType(int(int, bool, double) const volatile noexcept);
        testType(int(int, bool, double) const volatile&& noexcept);
        testType(int(int, bool, double,...) const volatile&& noexcept);
        testType(int(int, bool, double)& noexcept);

         #undef testType
    };
}



template<bool a, typename T, bool b, typename U>
struct myType{
    const int& f(int,bool);
    const volatile double&& h(const int&, bool, myType<0,float,0,float>&) const volatile;
    volatile int g;
};



namespace compStringNS::compStringConvNS::templatedTypeDefinitions{

template<bool a, typename T, bool b, typename U>
struct templatedTypeToCompString<myType<a,T,b,U>>{
    using typeList = mp_list<valueAsType<a>,T,valueAsType<b>,U>;
    using typeName = decltype("myType"_compStr);
    
};


}

template<typename T>
int render();

namespace{
    template<typename T>
    static constexpr auto tToSv = typeToCompString<T>::type::sv;
    #define testType(x) static_assert(tToSv<x> == #x""sv,tToSv<x>);
    struct TEST_template_types_toString{
        using t0 = myType<false,int,true,long int>;
        static_assert(tToSv<t0> == "myType<false, int, true, long int>"sv);
        using t1 = t0[321];
        static_assert(tToSv<t1> == "myType<false, int, true, long int>[321]"sv);
        using t2 = decltype(&t0::f);
        //static constexpr int h= render<t2>();
        static_assert(tToSv<t2> == "const int&(myType<false, int, true, long int>::*)(int, bool)"sv);
        using t3 = decltype(&t0::g);
       // static constexpr int hw= render<t3>();
        static_assert(tToSv<t3> == "volatile int myType<false, int, true, long int>::*"sv);
        using t4 = decltype(&t0::h)* const volatile** const*** volatile****;
        //static constexpr int hw= render<t4>();
        static_assert(tToSv<t4> == "const volatile double&&(myType<false, int, true, long int>::** const volatile** const*** volatile****)(const int&, bool, myType<false, float, false, float>&) const volatile"sv);
    };

    #undef testType
}

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
namespace{
using U0 = typename typeToCompString<templateTemplateParameterExampleType<int,void,double>>::type;
static_assert(U0::sv == "TTP Example Type<int, void, double>"sv);
using T0 = typename typeToCompString< myTemplatedType<templateTemplateParameterExampleType,char,5,int,bool,char> >::type;
static_assert(T0::sv == "myTemplatedType<TTP Example Type, char, 5, int, bool, char>"sv);

struct nonAddedT{}; // this type will mot be added to the convertible types

using U = typename typeToCompString<nonAddedT>::type;
static_assert(U::sv == "[no name given to type]");
using T = typename typeToCompString<const nonAddedT&>::type;
static_assert(T::sv == "const [no name given to type]&");
using H = typeToCompString<volatile nonAddedT* const* volatile*&&>::type;
static_assert(H::sv == "volatile [no name given to type]* const* volatile*&&");
}