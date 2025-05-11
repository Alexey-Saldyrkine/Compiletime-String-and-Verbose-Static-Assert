#include <string_view>
#include <iostream>
#include <type_traits>

#include <charconv>



namespace compStringNS{

namespace detail{
template <char... chars>
using tstring = std::integer_sequence<char, chars...>;

template <typename T, T... chars>
constexpr tstring<chars...> operator""_tstr() {
  return {};
}

template <typename T, T, typename>
struct prependIntSeq;

template <typename T, T chr, T... c>
struct prependIntSeq<T, chr, std::integer_sequence<T, c...>> {
  using type = std::integer_sequence<T, chr, c...>;
};

template<size_t i, size_t pos, char... c>
struct subSeqImpl{
    static constexpr auto f(){
        constexpr char str[sizeof...(c)] = {c...};
        if constexpr(i >= sizeof...(c) || i > pos){
            return std::integer_sequence<char>{};
        }else{
         return typename prependIntSeq<char,str[i],typename subSeqImpl<i+1,pos,c...>::type>::type{};
        }
    }

    using type = decltype(f());
};

template <size_t s, size_t e, char... c>
struct subSeq{
  static_assert(s <= e);

  using type =
      typename subSeqImpl<s,e,c...>::type;
};

template <size_t, char, typename>
struct findCharIndexImpl;

template <size_t index, char target, char cur, char... c>
struct findCharIndexImpl<index, target,
                         std::integer_sequence<char, cur, c...>> {
  static constexpr size_t value =
      (cur == target
           ? index
           : findCharIndexImpl<index + 1, target,
                               std::integer_sequence<char, c...>>::value);
};

template <size_t index, char target, char c>
struct findCharIndexImpl<index, target, std::integer_sequence<char, c>> {
  static constexpr size_t value = (target == c ? index : -1);
};

template <typename>
struct compString{};

template<typename,typename>
struct appendStringImpl;

template<char... a,char... b>
struct appendStringImpl<tstring<a...>,tstring<b...>>{
    using type = compString<tstring<a...,b...>>;
};

template<char... a,char... b>
struct appendStringImpl<tstring<a...>,compString<tstring<b...>>>{
    using type = compString<tstring<a...,b...>>;
};

template<typename,typename>
struct prependStringImpl;

template<char... a,char... b>
struct prependStringImpl<tstring<a...>,tstring<b...>>{
    using type = compString<tstring<b...,a...>>;
};

template<char... a,char... b>
struct prependStringImpl<tstring<a...>,compString<tstring<b...>>>{
    using type = compString<tstring<b...,a...>>;
};

template<size_t a, size_t b>
constexpr size_t ctMin = (a<=b?a:b);

template<size_t i, typename str, typename pattern, int dif>
struct findImpl{
    
    static constexpr size_t f(){
        if constexpr( i > str::size - pattern::size){
            return -1ull;
        }else{
        using sub = typename str::substr<i,pattern::size>;
        if constexpr(sub::size < pattern::size){
            return -1ull;
        }
        if constexpr(sub::template equal<pattern>){
            return i;
        }else{
            return findImpl<i+dif,str,pattern,dif>::value;
        }
        }
    }
    static constexpr size_t value = f();
};

template<size_t i, typename str, int dif, bool invert, char... c>
struct findFirstOfImpl{
   
    static constexpr size_t f(){
        if constexpr(i >= str::size){
            return -1ull;
        }else if constexpr (!invert&&(((str::template at<i>)==c)||...) || invert&&(!(((str::template at<i>)==c)||...))){
            return i;
        }else{
            return findFirstOfImpl<i+dif,str,dif,invert,c...>::value;
        }
    }
    static constexpr size_t value = f();
};

template<size_t i, typename str,typename T, int dif, bool invert>
struct findFirstOfInter;

template<size_t i, typename str, int dif,bool invert, char... c>
struct findFirstOfInter<i,str,compString<tstring<c...>>,dif,invert>{
    static constexpr size_t value = findFirstOfImpl<i,str,dif,invert,c...>::value;
};

template<typename A, typename B, size_t pos1 = -1, size_t count1 =(A::size - pos1), size_t pos2=-1, size_t count2 =(B::size - pos2)>
struct compareCompString{
    static constexpr int f(){
       using strA = typename A::template substr<(pos1 == -1 ? 0 : pos1),(pos1 == -1 ? A::size : pos1 + count1)>;
       using strB = typename B::template substr<(pos2 == -1 ? 0 : pos2),(pos2 == -1 ? B::size : pos2 + count2)>;
       if(strA::sv == strB::sv){
            return 0;
       }
       if(strA::sv > strB::sv){
            return 1;
       }else{
            return -1;
       }
    }
    static constexpr int value = f();
};


template<typename R, typename... Args>
struct funcSignature{};

template<typename>
struct to_signature;

template<typename T>
using to_signature_t = typename to_signature<T>::type;

template<typename R, typename... Args>
struct to_signature<R(Args...)>{
    using type = funcSignature<R,Args...>;
};

template<typename R,typename T, typename... Args>
struct to_signature<R(T::*)(Args...) const>{
    using type = funcSignature<R,Args...>;
};

template<size_t i,typename str, typename pred>
struct erase_ifImpl{

     static constexpr bool invokePred(){
        using predSignature = to_signature_t<decltype(&pred::operator())>;
        if constexpr (std::is_same_v<to_signature_t<bool(size_t)>, predSignature  >){ //std::is_invocable_v<pred,size_t>
            return pred{}(i);
        }else if constexpr (std::is_same_v<to_signature_t<bool(char)>, predSignature  >){
            return pred{}(str::template at<i>);
        }else if constexpr (std::is_same_v<to_signature_t<bool(size_t,char)>, predSignature  >){
            return pred{}(i,str::template at<i>);
        }else if constexpr (std::is_same_v<to_signature_t<bool(char,size_t)>, predSignature  >){
            return pred{}(str::template at<i>,i);
        }else{
            throw "copmString::erase_if was given a predicate that does not have the signature of: bool(char) || bool(size_t) || bool(char,size_t) || bool(size_t,char)";
            return -1;
        }
    }
    static constexpr auto f(){
        static_assert(i<= str::size);
        if constexpr(i >= str::size){
            return typename str::clear{};
        }else
        if constexpr(!invokePred()){
             return typename appendStringImpl<tstring<str::template at<i>>, typename erase_ifImpl<i+1,str,pred>::type>::type{};
        }else{
            return typename erase_ifImpl<i+1,str,pred>::type{};
        }
    }

    using type = decltype(f());
};


template<size_t i, typename str, typename replaceFunc>
struct replace_ifImpl{

    static constexpr char invokeReplaceFunc(){
        using replaceFuncSignature = to_signature_t<decltype(&replaceFunc::operator())>;
        if constexpr (std::is_same_v<to_signature_t<char(size_t)>, replaceFuncSignature  >){ //std::is_invocable_v<pred,size_t>
            return replaceFunc{}(i);
        }else if constexpr (std::is_same_v<to_signature_t<char(char)>, replaceFuncSignature  >){
            return replaceFunc{}(str::template at<i>);
        }else if constexpr (std::is_same_v<to_signature_t<char(size_t,char)>, replaceFuncSignature  >){
            return replaceFunc{}(i,str::template at<i>);
        }else if constexpr (std::is_same_v<to_signature_t<char(char,size_t)>, replaceFuncSignature  >){
            return replaceFunc{}(str::template at<i>,i);
        }else{
            throw "copmString::replace_if was given a replaceFunc that does not have the signature of: char(char) || char(size_t) || char(char,size_t) || char(size_t,char)";
            return -1;
        }
    }

    static constexpr auto f(){
        static_assert(i<= str::size);
        if constexpr(i >= str::size){
            return typename str::clear{};
        }else{
            return typename appendStringImpl<tstring<invokeReplaceFunc()>, typename replace_ifImpl<i+1,str,replaceFunc>::type>::type{};
        }
        
    }

    using type = decltype(f());
};

template<size_t i, typename emptystr>
constexpr auto toCompStringImpl(std::string_view s){
    if constexpr(s.size() >= i){
        return typename emptystr::clear{};
    }else{
        return typename appendStringImpl<tstring<s[i]>,decltype(toCompStringImpl<i+1,emptystr>(s)) >::type{};
    }
}
template<typename T>
struct typeWrapper{
    using type = T;
};

template<typename str,size_t i>
struct compStringReverseImpl{
    static constexpr auto f(){
        if constexpr(str::size == 0){
            return typeWrapper<str>{};
        }else
        if constexpr(i < str::size-1){
            using t = typename compStringReverseImpl<str,i+1>::type::type;
            using retType = typename t::push_back<str::template at<i>>;
            return typeWrapper<retType>{};
        }else{
            using retType = compString<detail::tstring< str::template at<str::size-1> > >;
            return typeWrapper<retType>{};
        }
    }
    using type = decltype(f());
};

template<typename str>
struct compStringReverse{
    using inter = typename compStringReverseImpl<str,0>::type;
    using type  = typename inter::type;
};

}
using detail::compString;
template <char... c>
struct compString<detail::tstring<c...>> {

  

  using thisStr = compString<detail::tstring<c...>>;
  static constexpr char str[sizeof...(c) + 1] = {c..., '\0'};
  static constexpr size_t size = sizeof...(c);
  template <size_t r, size_t l>
  using substrLR = compString<
      typename detail::subSeq<r, l, c...>::type>;

  template<typename str>
    using append = typename detail::appendStringImpl<detail::tstring<c...>,str>::type;
  template<typename str>
    using prepend = typename detail::prependStringImpl<detail::tstring<c...>,str>::type;

    // Element access
    template<size_t pos>
    static constexpr char at = str[pos];
    static constexpr const char* c_str = str;
    static constexpr char front = at<0>;
    static constexpr char back = at<size-1>;

    static constexpr const char* data = c_str;
    static constexpr const auto to_basic_string_view = std::basic_string_view(str,size);
    static constexpr const auto sv = to_basic_string_view;
    //Iterators
    //Capacity
    //Modifiers
    using clear = compString<decltype(""_tstr)>;
    template<size_t pos, typename str> 
    using insert = typename substrLR<0,pos>::append<str>::append<substrLR<pos+1,size-1>>;
    template<size_t pos, size_t count>
    using erase = typename substrLR<0,pos>::append<substrLR<ctMin<pos+count+1,size>,size>>;
    template<char chr>
    using push_back = append<compString<tstring<chr>>>;
    using pop_back = substrLR<0,size-2>;
    template<size_t pos, size_t count, typename str>
    using replace = typename erase<pos,count>::insert<pos,str>;
    //search
    template<typename str,size_t pos = 0>
    static constexpr size_t find = findImpl<pos, thisStr,str,1>::value;
    template<typename str, size_t pos = size-str::size>
    static constexpr size_t rfind = findImpl<pos,thisStr,str,-1>::value;
    template<typename str, size_t pos =0>
    static constexpr size_t find_first_of = findFirstOfInter<pos,thisStr,str,1,0>::value;
    template<typename str, size_t pos =0>
    static constexpr size_t find_first_not_of = findFirstOfInter<pos,thisStr,str,1,1>::value;
    template<typename str, size_t pos = size-1>
    static constexpr size_t find_last_of = findFirstOfInter<pos,thisStr,str,-1,0>::value;
    template<typename str, size_t pos = size-1>
    static constexpr size_t find_last_not_of = findFirstOfInter<pos,thisStr,str,-1,1>::value;
    //Operations
    template<size_t pos, size_t count>
    using substr = substrLR<pos,pos+count-1>;
    template<typename str,size_t pos1 = -1ull, size_t count1 = size - pos1, size_t pos2 = -1ull, size_t count2 = str::size - pos2>
    static constexpr int compare = compareCompString<thisStr,str,pos1,count1,pos2,count2>::value;
    template<typename str>
    static constexpr bool starts_with = find<str> == 0;
    template<typename str>
    static constexpr bool ends_with = rfind<str> == size - str::size;
    template<typename str>
    static constexpr bool contains = find<str> != -1ull;
    //other
    template<typename str>
    static constexpr bool equal = (sv == str::sv);
    template<typename str>
    static constexpr bool lesser = (size == str::size ?(sv < str::sv):size < str::size);
    template<typename str>
    static constexpr bool greater = (size == str::size ?(sv > str::sv):size > str::size);
    template<typename str>
    static constexpr bool lesserEq = equal<str> || lesser<str>;
    template<typename str>
    static constexpr bool greaterEq = equal<str> || greater<str>;
    template<typename pred>
    using erase_if = typename erase_ifImpl<0,thisStr,pred>::type;
    template<typename replaceFunc>
    using replace_if = typename replace_ifImpl<0,thisStr,replaceFunc>::type;

    static constexpr size_t sizeInterm = size;
    struct out{
        const char * data = str;
        size_t size = sizeInterm;
    };

    static constexpr out outobj = out{};
};

template <typename T, T... chars>
constexpr compString<detail::tstring<chars...>> operator""_compStr() {
  return {};
}

}

namespace compStringNS{
namespace compStringConvNS{
template<typename T>
struct typeToCompString{
    using type = decltype("[no name given]"_compStr);
};

template<>
struct typeToCompString<int>{
    using type = decltype("int"_compStr);
};

#define createTypeNameDefinition(x) template<> struct typeToCompString<x>{ using type = decltype(#x ""_compStr);};

createTypeNameDefinition(char);



template<typename T, auto val>
struct valueToCompStringInter{
    using type = decltype("[no name given]"_compStr);
};



template<typename T, T val,typename str>
struct integerValToCompString{
    static constexpr auto f(){
        if constexpr(val >=10){
        constexpr T i = val%10;
        using t = str::template push_back<'0'+i>;
        return typename integerValToCompString<T,val/10,t>::type{};
        }else{
             using t = str::template push_back<static_cast<char>('0'+val)>;
             return t{};
        }
    }
    using inter = decltype(f());
    using type = typename detail::compStringReverse<inter>::type;
};

template<auto val>
struct integerValToCompStringInterm{
    using T = decltype(val);
    static constexpr auto f(){
        if constexpr(val >0){
            using t = typename integerValToCompString<T,val,decltype(""_compStr)>::type;
            return t{};
        }else{
            using t = typename integerValToCompString<T,-val,decltype(""_compStr)>::type;
            return typename t::template prepend<decltype("-"_compStr)>{};
        }
    }
    
    
    using type = decltype(f());
};


template<auto val>
struct valueToCompString{
    static constexpr auto f(){
        using T = decltype(val);
        if constexpr(std::is_integral_v<T>){
            return typename integerValToCompStringInterm<val>::type{};
        }else{
            return typename valueToCompStringInter<decltype(val),val>::type{};
        }
    }
    using type = decltype(f());   
};



template<typename T,size_t i, size_t size,const char* buf>
struct charArrayToType{
    static constexpr auto f(){
        if constexpr(i<size){
            using t = typename T::template push_back<*(buf+i)>;
            return typename charArrayToType<t,i+1,size,buf>::type{};
        }else{
            return T{};
        }
    }

    using type = decltype(f());
};

template<const char* buf>
constexpr size_t getCstrLength(){
    size_t ret=0;
    while(*(buf+ret) != '\0') ret++;
    return ret;
}

template<const char* buf >
struct valueToCompStringInter<const char*,buf>{
    using type = charArrayToType<decltype(""_compStr),0,getCstrLength<buf>(),buf>::type;
};




template<template<typename...> typename Caller>
struct templateNames{
    using name = decltype("undefiend name"_compStr);
    //static_assert(0);
};

#define createTemplateNameDefinition(x) template<> struct templateNames<x>{ using name = decltype(#x ""_compStr);};


template<>
struct templateNames<std::is_same>{
    using name = decltype("std::is_same"_compStr);
};

createTemplateNameDefinition(std::is_base_of);



template<typename T>
struct get_name{
    using name = typename typeToCompString<T>::type;
};


template<size_t i,size_t target,typename T,typename... Ts>
struct mp_list_at_impl{
    static constexpr auto f(){
        if constexpr(i == target){
            return T{};
        }else{
            return typename mp_list_at_impl<i+1,target,Ts...>::type{};
        }
    }
    using type = decltype(f());
};

template<size_t i,size_t target,typename... Ts>
struct mp_list_at_intern{

    //static_assert(i< sizeof...(Ts),)
};

template<typename... T>
struct mp_list{
    //template<size_t i>

};

template<template<typename...> typename caller,typename...T>
struct get_name<caller<T...>>{
    using name = typename templateNames<caller>::name;
    static constexpr size_t argCount = sizeof...(T);

};



}
}







namespace {
using namespace std::literals;
using namespace compStringNS;
struct TESTS{
    
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



namespace verbose_static_assertNS{
using namespace compStringNS;
using namespace compStringNS::compStringConvNS;
template<typename T>
struct verbose_static_assert{
    static constexpr const char* nameOfType = get_name<T>::name::str;
    static constexpr auto value = T::value;

    //static_assert(value,get_name<T>::name::sv);

    static void f(){
        std::cout<<"name: "<<nameOfType<<std::endl;
        std::cout<<"value: "<<value<<std::endl;
    }
};
}




constexpr const char cstr1[]="hello";
constexpr const char cstr2[]="this is a cstr";
int main() {
    using namespace std;
    using namespace compStringNS;
    using namespace compStringNS::compStringConvNS;
    
    cout<<typeToCompString<int>::type::sv<<"$"<<endl;
    cout<<valueToCompString<234>::type::sv<<"$"<<endl;
    cout<<valueToCompString<cstr1>::type::sv<<"$"<<endl;
    cout<<valueToCompString<cstr2>::type::sv<<"$"<<endl;


    cout<<endl;
    cout<<endl;
    using namespace verbose_static_assertNS;
   verbose_static_assert<std::is_same<int,char>>::f();
   verbose_static_assert<std::is_base_of<int,char>>::f();

    return 0;
}