#include "compileTime String.hpp"
#include <iostream>




int main() {
    using namespace std;
    using namespace compStringNS;
    using str = decltype("hello world"_compStr);
    using our = decltype("our "_compStr);
    using my = decltype("my "_compStr);
    using l = decltype("l"_compStr);
    using ourworld = str::insert<5,our>;
    cout<<str::substr<0,4>::sv<<endl;

     cout<<str::sv<<endl;
     cout<<ourworld::sv<<endl;
     cout<<ourworld::erase<4,4>::sv<<endl;
     cout<<ourworld::erase<4,999>::sv<<endl;
     cout<<str::push_back<'s'>::sv<<endl;
     cout<<str::push_back<'s'>::pop_back::sv<<endl;
     cout<<ourworld::replace<5,4,my>::sv<<endl;
     using myworld = ourworld::replace<5,4,my>;
     cout<<ourworld::find<l><<endl;
     cout<<ourworld::rfind<decltype("l"_compStr)><<endl;
     cout<< myworld::find_first_of<decltype("ym"_compStr)> << endl;
     cout<< myworld::find_last_of<decltype("my"_compStr)> << endl;
     cout<< myworld::find_first_not_of<decltype("hl eo"_compStr)> << endl;
     cout<< myworld::find_last_not_of<decltype("dlw or"_compStr)> << endl;
     using strA = decltype("abc"_compStr);
     cout<< strA::compare<decltype("abc"_compStr)> <<endl;

     using predB= decltype( [](size_t i, char c)->bool{
        return c=='l' && i%2;
     });

     cout<< str::erase_if<predB>::sv <<endl;

     using replaceFunc = decltype([](char c, size_t i)->char{
                if(i%2==0 || c ==' ')
                    return c;
                else
                    return'Y';
            });
    cout<<str::replace_if<replaceFunc>::sv<<endl;
    
}