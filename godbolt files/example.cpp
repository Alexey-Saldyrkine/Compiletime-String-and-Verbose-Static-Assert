#include "compString.hpp" 
#include "compString_tests.hpp"
#include "compString_conversion.hpp"
#include "compString_conversion_tests.hpp"
//#include "verbose_static_assert.hpp"
//#include "VSA_examplesAndTests.hpp"


int main() {

   using t = decltype("my"_compStr);
   static_assert(t::sv == "my"sv);
   static_assert(t::sv != "ym"sv);
   
}