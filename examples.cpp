#include "compileTime String.hpp"
#include <iostream>


namespace example{
    using namespace std;
    using namespace compStringNS;

    struct baseErrorMsg{
        using name = decltype("[No name given]"_compStr);
        using error1detail = decltype("[No msg given]"_compStr);
    };

    struct A:baseErrorMsg{
        using name = decltype("A"_compStr);
        using error1detail = decltype("X"_compStr);
    };
    struct B:baseErrorMsg{
        using name = decltype("B"_compStr);
        using error1detail = decltype("Y"_compStr);
    };
    struct C:baseErrorMsg{

    };

    template<typename T>
    struct derived:T{
        
        using error1msg = typename 
        decltype("Error1, occurred in base \'"_compStr)
        ::append<typename T::name>
        ::template append<decltype("\', because: "_compStr)>
        ::append<typename T::error1detail>;


        void printName(){
            using msg = decltype("Type is derived from "_compStr)::append<typename T::name>;
            cout<<msg::sv<<endl;
        }
        
        void printError1(){
            
            cout<<error1msg::sv<<endl;
        }
    };
}


int main() {
    using namespace example;

    derived<A> a;
    derived<B> b;

    a.printName();
    b.printName();
    //a.printError1();
    b.printError1();
   static_assert(0,derived<A>::error1msg::sv);
   static_assert(0,derived<B>::error1msg::sv);
   static_assert(0,derived<C>::error1msg::sv);

    return 0;
}