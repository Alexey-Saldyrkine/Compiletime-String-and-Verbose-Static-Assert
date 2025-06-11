#include "compileTime String.hpp"

namespace {
using namespace std::literals;
using namespace compStringNS;
struct compStringTests{
    
    using str = decltype("hello world"_compStr);
    using our = decltype("our "_compStr);
    using my = decltype("my "_compStr);
    using ourworld = str::insert<5,our>;
    static_assert(str::sv == "hello world"sv);
    //clear
    static_assert(str::clear::sv == ""sv);
    // insert
    static_assert(ourworld::sv == "hello our world"sv);
    //erase
    static_assert(ourworld::erase<4,4>::sv == "hello world"sv);
    static_assert(ourworld::erase<4,99>::sv == "hello"sv);
    //push_back & pop_back
    static_assert(str::push_back<'s'>::sv == "hello worlds"sv);
    static_assert(str::pop_back::sv == "hello worl"sv);
    //replace
    static_assert(ourworld::replace<5,4,my>::sv == "hello my world"sv);
    //find variants
    static_assert(ourworld::find<decltype("l"_compStr)> == 2);
    static_assert(ourworld::find<decltype("z"_compStr)> == -1ull);
    static_assert(ourworld::rfind<decltype("l"_compStr)> == 13);
    using myworld = ourworld::replace<5,4,my>;
    static_assert(myworld::find_first_of<decltype("ym"_compStr)> == 6);
    static_assert(myworld::find_last_of<decltype("my"_compStr)> == 7);
    static_assert(myworld::find_first_not_of<decltype("hl eo"_compStr)> == 6);
    static_assert(myworld::find_last_not_of<decltype("dlw or"_compStr)> == 7);
    //compare
    using strA = decltype("abc"_compStr);
    static_assert(strA::compare<decltype("abc"_compStr)> == 0);
    static_assert(strA::compare<decltype("abcs"_compStr)> == -1);
    static_assert(strA::compare<decltype("ab"_compStr)> == 1);
    static_assert(strA::compare<decltype("abb"_compStr)> == 1);
    static_assert(strA::compare<decltype("aac"_compStr)> == 1);
    static_assert(strA::compare<decltype("abd"_compStr)> == -1);
    static_assert(strA::compare<decltype("bbc"_compStr)> == -1);
    static_assert(strA::compare<decltype("ab"_compStr),0,2> == 0);
    static_assert(strA::compare<decltype("abc"_compStr),0,2> == -1);
    static_assert(strA::compare<decltype("abc"_compStr),0,2,0,2> == 0);
    static_assert(strA::compare<decltype("abc"_compStr),0,2,0,1> == 1);
    static_assert(strA::compare<decltype("aac"_compStr),0,2,0,2> == 1);
    static_assert(strA::compare<decltype("abc"_compStr),-1ull,0,-1ull,0> == 0);
    //other compares
    static_assert(decltype("hello"_compStr)::equal<decltype("hello"_compStr)> == 1);
    static_assert(decltype("hello"_compStr)::equal<decltype("hello "_compStr)> == 0);
    static_assert(decltype("hello"_compStr)::lesser<decltype("zzz"_compStr)> == 0);
    static_assert(decltype("hello"_compStr)::lesserEq<decltype("zzz"_compStr)> == 0);
    static_assert(decltype("hello"_compStr)::lesser<decltype("aaaaaa"_compStr)> == 1);
    static_assert(decltype("hello"_compStr)::greater<decltype("zzzzzz"_compStr)> == 0);
    static_assert(decltype("hello"_compStr)::greaterEq<decltype("zzzzzz"_compStr)> == 0);
    static_assert(decltype("hello"_compStr)::greater<decltype("aaa"_compStr)> == 1);
    static_assert(decltype("hello"_compStr)::greaterEq<decltype("hello"_compStr)> == 1);
    static_assert(decltype("hello"_compStr)::lesserEq<decltype("hello"_compStr)> == 1);
    //starts_with & ends_with
    static_assert(strA::starts_with<decltype("a"_compStr)> == 1);
    static_assert(strA::starts_with<decltype("ab"_compStr)> == 1);
    static_assert(strA::starts_with<decltype("b"_compStr)> == 0);
    static_assert(strA::starts_with<decltype("bc"_compStr)> == 0);
    static_assert(strA::ends_with<decltype("c"_compStr)> == 1);
    static_assert(strA::ends_with<decltype("bc"_compStr)> == 1);
    static_assert(strA::ends_with<decltype("b"_compStr)> == 0);
    static_assert(strA::ends_with<decltype("ab"_compStr)> == 0);
    //conatins
    static_assert(strA::contains<decltype("a"_compStr)> == 1);
    static_assert(strA::contains<decltype("ab"_compStr)> == 1);
    static_assert(strA::contains<decltype("bc"_compStr)> == 1);
    static_assert(strA::contains<decltype("abc"_compStr)> == 1);
    static_assert(strA::contains<decltype("d"_compStr)> == 0);
    static_assert(strA::contains<decltype("ad"_compStr)> == 0);
    static_assert(strA::contains<decltype("acb"_compStr)> == 0);
    //erase_if
    using predA= decltype( [](char c)->bool{
        return c =='l';
     });
     static_assert(str::erase_if<predA>::sv == "heo word"sv);
    using predB= decltype( [](size_t i)->bool{
        return i%2;
     });
     static_assert(str::erase_if<predB>::sv == "hlowrd"sv);
    using predC= decltype( [](size_t i, char c)->bool{
        return c=='l' && i%2;
     });
    static_assert(str::erase_if<predC>::sv == "helo word"sv);
     using predD= decltype( [](char c, size_t i)->bool{
        return c=='l' && i%2;
     });
     static_assert(str::erase_if<predD>::sv == "helo word"sv);

    // replace_if
    using replaceFuncA = decltype([](char c)->char{
            if(c == 'l')
                return 'z';
            else
                return c;
            });
    static_assert(str::replace_if<replaceFuncA>::sv == "hezzo worzd"sv);
    using replaceFuncB = decltype([](size_t i)->char{
                return 'a'+i;
            });
    static_assert(str::replace_if<replaceFuncB>::sv == "abcdefghijk"sv);
    using replaceFuncC = decltype([](char c, size_t i)->char{
                if(i%2==0 || c ==' ')
                    return c;
                else
                    return'Y';
            });
    static_assert(str::replace_if<replaceFuncC>::sv == "hYlYo wYrYd"sv);
    using replaceFuncD = decltype([](size_t i, char c)->char{
                if(i%2==0 || c ==' ')
                    return c;
                else
                    return'Y';
            });
    static_assert(str::replace_if<replaceFuncD>::sv == "hYlYo wYrYd"sv);

};
}


namespace{
    using namespace std::literals;
    using namespace compStringNS;
    using namespace compStringNS::compStringConvNS;
    template<auto V>
    constexpr auto vToSV = valueToCompString<V>::type::sv;
    struct compStringConvTestsValue{
        #define vToSvTestI(x) static_assert(vToSV<x> == #x""sv,vToSV<x>);
        #define vToSvTest(x,str) static_assert(vToSV<x> == #str""sv,vToSV<x>);
        //integer to compString
        static_assert(vToSV<0> == "0"sv);
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

        // const char*

        #define constCharPTests(n,x) static constexpr const char TTP_##n []= x; static_assert( vToSV<TTP_##n> == x""sv);

        constCharPTests(a,"hello world, abra, snore, galaxy world");
        constCharPTests(b,"23465134rsadf2gqraskfiq432t wmetrntq2u3nr v  m3ir 23trm w4it 20it imweiomt23mr i");
    };

    struct compStringConvTypes{
        template<typename T>
        static constexpr auto tToSv = typeToCompString<T>::type::sv;
        #define testType(x) static_assert(tToSv<x> == #x""sv,tToSv<x>);
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
    };
}