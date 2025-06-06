#include <string_view>
#include <iostream>
#include <type_traits>
#include <utility>


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
    template<size_t i = 0>
    using pop_back = substrLR<i,size-2>;
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
    template<typename str = thisStr>
    using reverse = compStringReverse<str>::type;
};

template <typename T, T... chars>
constexpr compString<detail::tstring<chars...>> operator""_compStr() {
  return {};
}

}

namespace compStringNS{
namespace compStringConvNS{



namespace typeToStringDefinitions{

template<typename T>
struct typeToCompStringInter{
    using type = decltype("[no name given to type]"_compStr);
};

#define createTypeDefinition(x) template<> struct typeToCompStringInter<x>{ using type = decltype(#x ""_compStr);};

createTypeDefinition(void);
createTypeDefinition(bool);
createTypeDefinition(char);
createTypeDefinition(signed char);
createTypeDefinition(unsigned char);
createTypeDefinition(short int);
createTypeDefinition(unsigned short int);
createTypeDefinition(int);
createTypeDefinition(unsigned int);
createTypeDefinition(long int);
createTypeDefinition(unsigned long int);
createTypeDefinition(long long int);
createTypeDefinition(unsigned long long int);
createTypeDefinition(float);
createTypeDefinition(double);
createTypeDefinition(long double);
}

template<typename T>
struct typeToCompString;

template<auto val>
struct valueToCompString;

namespace detailPPD{

template<typename T>
struct type_wrapper{
    using type = T;
};

template<size_t i,size_t target,typename T,typename... Ts>
struct mp_list_at_impl{
    static constexpr auto f(){
        if constexpr(sizeof...(Ts) == 0){
            return type_wrapper<T>{};
        }else
        if constexpr(i == target){
            return type_wrapper<T>{};
        }else{
            return typename mp_list_at_impl<i+1,target,Ts...>::type{};
        }
    }
    using type = decltype(f());
};

template<typename... T>
struct mp_list{
    static constexpr size_t size = sizeof...(T);

    template<size_t i>
    using at =typename mp_list_at_impl<0,i,T...>::type::type;

    template<typename... other>
    using append = mp_list<T...,other...>;
};


template<auto val>
struct valueAsType{
    using type = decltype(val);
    static constexpr type value = val;
};

template<typename T>
struct isTypeImpl{
    static constexpr bool value = true;
};
template<auto val>
struct isTypeImpl<valueAsType<val>>{
    static constexpr bool value = false;
};

template<bool falseV,typename T, size_t i>
struct typeAtImplFailerMsg{
    using type = typename T::template at<i>;
};

template<typename T, size_t i>
struct typeAtImplFailerMsg<false,T,i>{
    using vWrap = typename T::template at<i>;
    using msg = decltype("VSA_TPP_data::typeAt - tried to access template parameter at pos "_compStr)
    ::template append<typename valueToCompString<i>::type>
    ::template append<decltype(" is not a type, but a vlaue of type \'"_compStr)>
    ::template append<typename typeToCompString<typename vWrap::type>::type>
    ::template append<decltype("\' and eqaul to ")>
    ::template append<typename valueToCompString<vWrap::value>::type>
    ::template append<decltype("\n"_compStr)>;

    static_assert(false,msg::sv);

};


template<typename T,bool isType, size_t indx>
struct typeAtImpl{
    using type = typeAtImplFailerMsg<isType,T,indx>::type;
};

template<bool falseV,typename T,size_t i>
struct valueAtImplFailerMsg{
    static constexpr auto value = T::template at<i>::value;
};

template<typename T, size_t i>
struct valueAtImplFailerMsg<false,T,i>{
    using msg = decltype("VSA_TPP_data::valueAt -  tried to access template parameter at pos "_compStr)
    ::template append<typename valueToCompString<i>::type>
    ::template append<decltype(" is not a value, but a type \'"_compStr)>
    ::template append<typename typeToCompString<typename T::template at<i>>::type>
    ::template append<decltype("\'\n"_compStr)>;


    static_assert(false,msg::sv);
};

template<typename T, bool isValue, size_t indx>
struct valueAtImpl{
    static constexpr auto value = valueAtImplFailerMsg<isValue,T,indx>::value;
};

template<bool isValue,typename T, size_t i>
struct stringAt_impl{
    static constexpr auto f(){
        if constexpr(isValue){
            return typename valueToCompString<valueAtImpl<T,true,i>::value>::type{};
        }else{
            return typename typeToCompString<typename typeAtImpl<T,true,i>::type>::type{}; 
        }
    }

    using type = decltype(f());
};

template<typename T,typename containerType>
struct detail_TPPD{
    static constexpr size_t size = T::size;
    template<size_t i>
    static constexpr bool isType = isTypeImpl<typename T::template at<i>>::value;
    template<size_t i>
    static constexpr bool isValue = !isType<i>;
    template<size_t i>
    using typeAt = typename typeAtImpl<T,isType<i>,i>::type;
    template<size_t i>
    using typeStringAt = typename typeToCompString<typeAt<i>>::type;
    template<size_t i>
    static constexpr auto valueAt = valueAtImpl<T,isValue<i>,i>::value;
    template<size_t i>
    using valueStringAt = typename valueToCompString<valueAt<i>>::type;
    template<size_t i>
    using stringAt = typename stringAt_impl<isValue<i>,T,i>::type;
    using container = containerType;
};

}


namespace templatedTypeDefinitions{
using detailPPD::valueAsType;
using detailPPD::mp_list;

template<typename T>
struct templatedTypeToCompString:std::false_type{
    using typeList = void; // should never be used
    using typeName = decltype("not a template type"_compStr);
};

}

namespace detail{

template<typename T,bool>
struct typeToCompStringInterm;

template<typename T>
struct typeToCompString_cv{
    static constexpr auto f(){
        if constexpr(std::is_const_v<T> && std::is_volatile_v<T>){
            return decltype("const volatile"_compStr){};
        }else
        if constexpr(std::is_volatile_v<T>){
            return decltype("volatile"_compStr){};
        }else
        if constexpr(std::is_const_v<T>){
            return decltype("const"_compStr){};
        }else{
            return decltype(""_compStr){};
        }
    }

    using type = decltype(f());
};

template<typename msg,typename T, typename... Ts>
struct typesToStringComma{
    static constexpr auto f(){  
        using str = typename typeToCompStringInterm<T,false>::type;
        using newMsg = typename msg::template append<str>;
        if constexpr(sizeof...(Ts) != 0){
            using retMsg = typename newMsg::template append<decltype(","_compStr)>;
            return typename typesToStringComma<retMsg,Ts...>::type{};
        }else{
            return newMsg{};
        }
    }

    using type = decltype(f());
};

template<typename R, typename... Args>
struct functionBase{
    static constexpr auto f(){
        using lHalf = typename typeToCompStringInterm<R,false>::type::template append<decltype("("_compStr)>;
        if constexpr(sizeof...(Args) == 0){
            return typename lHalf::template append<decltype(")"_compStr)>{};
        }else{
            using argsStr = typename typesToStringComma<decltype(""_compStr),Args...>::type;
            return typename lHalf::template append<argsStr>::template append<decltype(")"_compStr)>{};
        }
    }
    using type = decltype(f());
};

template<typename T>
struct functionBaseToVariadic{
    using type = typename T::template pop_back<>::template append<decltype("...)"_compStr)>;
};

template<typename T>
struct typeToCompString_function_type;


// basic function cv
template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...)>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = base;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) const>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = base::template append<decltype(" const"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) volatile>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = base::template append<decltype(" volatile"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) const volatile>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = base::template append<decltype(" const volatile"_compStr)>;
};

// basic function ref
// &
template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) &>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = base::template append<decltype("&"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) const &>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = base::template append<decltype(" const&"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) volatile &>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = base::template append<decltype(" volatile&"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) const volatile &>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = base::template append<decltype(" const volatile&"_compStr)>;
};
// &&

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) &&>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = base::template append<decltype("&&"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) const &&>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = base::template append<decltype(" const&&"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) volatile&&>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = base::template append<decltype(" volatile&&"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) const volatile&&>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = base::template append<decltype(" const volatile&&"_compStr)>;
};

// all above but variadic function

// basic function cv
template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args......)>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = base;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args......) const>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = base::template append<decltype(" const"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args......) volatile>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = base::template append<decltype(" volatile"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args......) const volatile>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = base::template append<decltype(" const volatile"_compStr)>;
};

// basic function ref
// &
template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args......) &>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = base::template append<decltype("&"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args......) const &>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = base::template append<decltype(" const&"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args......) volatile &>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = base::template append<decltype(" volatile&"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args......) const volatile &>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = base::template append<decltype(" const volatile&"_compStr)>;
};
// &&

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args......) &&>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = base::template append<decltype("&&"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args......) const &&>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = base::template append<decltype(" const&&"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args......) volatile&&>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = base::template append<decltype(" volatile&&"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args......) const volatile&&>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = base::template append<decltype(" const volatile&&"_compStr)>;
};


// all above but noexcept
// basic function cv
template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) noexcept>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = base::template append<decltype(" noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) const noexcept>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = base::template append<decltype(" const noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) volatile noexcept>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = base::template append<decltype(" volatile noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) const volatile noexcept>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = base::template append<decltype(" const volatile noexcept"_compStr)>;
};

// basic function ref
// &
template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) & noexcept>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = base::template append<decltype("& noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) const & noexcept>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = base::template append<decltype(" const& noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) volatile & noexcept>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = base::template append<decltype(" volatile& noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) const volatile & noexcept>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = base::template append<decltype(" const volatile& noexcept"_compStr)>;
};
// &&

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) && noexcept>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = base::template append<decltype("&& noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) const && noexcept>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = base::template append<decltype(" const&& noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) volatile&& noexcept>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = base::template append<decltype(" volatile&& noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) const volatile&& noexcept>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = base::template append<decltype(" const volatile&& noexcept"_compStr)>;
};

// all above but variadic function

// basic function cv
template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args......) noexcept>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = base::template append<decltype(" noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args......) const noexcept>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = base::template append<decltype(" const noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args......) volatile noexcept>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = base::template append<decltype(" volatile noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args......) const volatile noexcept>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = base::template append<decltype(" const volatile noexcept"_compStr)>;
};

// basic function ref
// &
template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args......) & noexcept>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = base::template append<decltype("& noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args......) const & noexcept>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = base::template append<decltype(" const& noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args......) volatile & noexcept>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = base::template append<decltype(" volatile& noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args......) const volatile & noexcept>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = base::template append<decltype(" const volatile& noexcept"_compStr)>;
};
// &&

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args......) && noexcept>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = base::template append<decltype("&& noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args......) const && noexcept>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = base::template append<decltype(" const&& noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args......) volatile&& noexcept>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = base::template append<decltype(" volatile&& noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args......) const volatile&& noexcept>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = base::template append<decltype(" const volatile&& noexcept"_compStr)>;
};

template<typename T>
struct pointerToFunction_type{
    using funcStr = typename typeToCompString_function_type<T>::type;
    static constexpr size_t pos = funcStr::template find<decltype("("_compStr)>;
    //using type = funcStr;
    using type = typename funcStr::template insert<pos-1,decltype("(*)"_compStr)>;
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
   // using inter =
    using type = decltype(f());//= typename detail::compStringReverse<inter>::type;
};

template<auto val>
struct integerValToCompStringInterm{
    using T = decltype(val);
    static constexpr auto f(){
        if constexpr(val >=0){
            using t = typename integerValToCompString<T,val,decltype(""_compStr)>::type;
            return typename compStringNS::detail::compStringReverse<t>::type{};
        }else{
            using t = typename integerValToCompString<T,-val,decltype(""_compStr)>::type;
            return typename compStringNS::detail::compStringReverse<t>::type::template prepend<decltype("-"_compStr)>{};
        }
    }
    
    
    using type = decltype(f());
};

template<const char* buf>
constexpr size_t getCstrLength(){
    size_t ret=0;
    while(*(buf+ret) != '\0') ret++;
    return ret;
};

template<bool is_pointer,typename T>
struct TTCS_removePointer_cond{
    using type = T;
};

template<typename T>
struct TTCS_removePointer_cond<true,T>{
    using type = std::remove_pointer_t<T>;
};

template<bool is_ref, typename T>
struct TTCS_removeRef_cond{
    using type = T;
};

template<typename T>
struct TTCS_removeRef_cond<true,T>{
    using type = std::remove_reference_t<T>;
};



template<bool, typename T>
struct SFNAI_If_apply{
    using type = T;
};

template<typename T>
struct req_remove_ref_pointer{
    static constexpr bool is_pointer = std::is_pointer_v<T>;
    static constexpr bool is_ref = std::is_reference_v<T>;
    using t = typename TTCS_removeRef_cond<is_ref,typename TTCS_removePointer_cond<is_pointer,T>::type>::type;
    static constexpr bool cond = std::is_pointer_v<t> || std::is_reference_v<t>;
   
    using type = typename SFNAI_If_apply<cond,t>::type;
};

template<typename T>
struct SFNAI_If_apply<true,T>{
    using type = typename req_remove_ref_pointer<T>::type;
};

template<typename T>
struct typeToCompString_is_base_a_function{
    using type = typename req_remove_ref_pointer<T>::type;
    static constexpr bool value = std::is_function_v<type>;
};




template<typename T, bool funcSigOverride>
struct typeToCompStringInterm{
    // steps to decompose a type:
    // 0. is function type (cuase ftypes are special)
    // 1. check refrences (noexcept for functions)
    // 2. check cv
    // 3. check pointer
    // if not base type, the repeat
    // now T will be a base type of some kind
    static constexpr auto f(){
            if constexpr(!funcSigOverride && detail::typeToCompString_is_base_a_function<T>::value){
                using ptrStr = typename typeToCompStringInterm<T,true>::type;
                using baseFunc = typename detail::typeToCompString_is_base_a_function<T>::type;
                using funcStr = typename detail::typeToCompString_function_type<baseFunc>::type;
                if constexpr(! ptrStr::template equal<decltype(""_compStr)>){
                    using fptr = typename decltype("("_compStr):: append<ptrStr>::template append<decltype(")"_compStr)>;
                    static constexpr size_t pos = funcStr::template find<decltype("("_compStr)>-1;
                    using retStr = typename funcStr::template insert<pos,fptr>;
                    return retStr{};
                }else{
                    return funcStr{};
                }
            }else
           if constexpr(funcSigOverride && std::is_function_v<T>){
                return decltype(""_compStr){};
                //return typename detail::typeToCompString_function_type<T>::type{};
           }else
           if constexpr(std::is_reference_v<T>){
                using t = std::remove_reference_t<T>;
                if constexpr(std::is_lvalue_reference_v<T>){
                    return typename typeToCompStringInterm<t,funcSigOverride>::type::template append<decltype("&"_compStr)>{};
                }else{
                    return typename typeToCompStringInterm<t,funcSigOverride>::type::template append<decltype("&&"_compStr)>{};
                }
           }else
           if constexpr(std::is_const_v<T> || std::is_volatile_v<T>){
                using cvStr = typename detail::typeToCompString_cv<T>::type;
                using t = std::remove_cv_t<T>;
                if constexpr(std::is_pointer_v<T>){
                    return typename typeToCompStringInterm<t,funcSigOverride>::type::template append<decltype(" "_compStr)>::template append<cvStr>{};
                }else{
                    using retStr =  cvStr::template append<decltype(" "_compStr)>::template append<typename typeToCompStringInterm<t,funcSigOverride>::type>;
                    return retStr{};
                }
           }else
           if constexpr(std::is_pointer_v<T>){
                using t = std::remove_pointer_t<T>;
                return typename typeToCompStringInterm<t,funcSigOverride>::type::template append<decltype("*"_compStr)>{};
                
           }else{
                
                    // fundamental and class types
                    return typename typeToStringDefinitions::typeToCompStringInter<T>::type{};
                
           }
    }
    using type = decltype(f());
};




template<size_t i, typename Data, typename msg>
struct templatedTypeToCompString_req{
    static constexpr auto f(){
        if constexpr( i >= Data::size){
            return msg{};
        }else
        if constexpr( i == Data::size-1){
            using retmsg = msg::template append<typename Data::template stringAt<i>>;
            return retmsg{};
        }else{
            using retmsg = msg::template append<typename Data::template stringAt<i>>::template append<decltype(","_compStr)>;
            return typename templatedTypeToCompString_req<i+1,Data,retmsg>::type{};
        }
    }
    using type = decltype(f());
};

template<typename T>
struct templatedTypeToCompString{
    using intermData = templatedTypeDefinitions::templatedTypeToCompString<T>;
    using templateName = typename intermData::typeName;
    using data = detailPPD::detail_TPPD<typename intermData::typeList,void>;

    using parameterStr = typename templatedTypeToCompString_req<0,data,decltype(""_compStr)>::type;

    using type = templateName::template append<decltype("<"_compStr)>::template append<parameterStr>::template append<decltype(">"_compStr)>;
};

template<typename T>
struct typeOrTemplateToCompString{
    static constexpr auto f(){
        if constexpr(templatedTypeDefinitions::templatedTypeToCompString<T>::value){
            return typename templatedTypeToCompString<T>::type{};
        }else{
            return typename detail::typeToCompStringInterm<T,false>::type{};
        }
    }

    using type = decltype(f());
};

}


template<typename T>
struct typeToCompString{
    using type = typename detail::typeOrTemplateToCompString<T>::type;

    template<typename = void>
    using templateName = typename templatedTypeDefinitions::templatedTypeToCompString<T>::typeName;
};



template<typename T, auto val>
struct valueToCompStringInter{
    using type = decltype("[no value to compString conversion for the type '"_compStr)::append<typename typeToCompString<T>::type>::template append<decltype("']"_compStr)>;
};



template<auto val>
struct valueToCompString{
    static constexpr auto f(){
        using T = decltype(val);
        if constexpr(std::is_same_v<bool,std::remove_cv_t<T>>){
            if constexpr(val == true){
                return decltype("true"_compStr){};
            }else{
                return decltype("false"_compStr){};
            }
        }else
        if constexpr(std::is_same_v<char,std::remove_cv_t<T>>){
            return compString<compStringNS::detail::tstring<val>>{};
        }else
        if constexpr(std::is_integral_v<T>){
            return typename detail::integerValToCompStringInterm<val>::type{};
        }else{
            return typename valueToCompStringInter<decltype(val),val>::type{};
        }
    }
    using type = decltype(f());   
};
template<typename T,size_t i, size_t size,const char* buf>
struct charArrayToTypeInter{
    static constexpr auto f(){
        if constexpr(i<size){
            using t = typename T::template push_back<*(buf+i)>;
            return typename charArrayToTypeInter<t,i+1,size,buf>::type{};
        }else{
            return T{};
        }
    }

    using type = decltype(f());
};



template<const char* buf, size_t len>
struct charArrayToType{
    using type = charArrayToTypeInter<decltype(""_compStr),0,len,buf>::type;
};

template<const char* buf >
struct valueToCompStringInter<const char*,buf>{
    using type = charArrayToType<buf,detail::getCstrLength<buf>()>::type;
};








// template<template<typename...> typename Caller>
// struct templatedTypeToCompString{
//     using type = decltype("[undefiend templated type name]"_compStr);
//     //static_assert(0);
// };

// #define createTemplatedTypeDefinition(x) template<> struct templatedTypeToCompString<x>{ using type = decltype(#x ""_compStr);};


// template<>
// struct templatedTypeToCompString<std::is_same>{
//     using type = decltype("std::is_same"_compStr);
// };

// createTemplatedTypeDefinition(std::is_base_of);

}
}





namespace verbose_static_assertNS{
using namespace compStringNS;
using namespace compStringNS::compStringConvNS;

template<typename T,typename containerType = void>
using VSA_template_parameter_pack_data = detailPPD::detail_TPPD<T,containerType>;

namespace detail{

using namespace compStringNS::compStringConvNS::detailPPD;

template<typename T>
struct test_if_is_VSA_template_paremeter_pack_data_Impl{
    using msg = decltype("given type is not a VAS_tempalte_parameter_pack_data type, it is the type \'"_compStr)
    ::template append<typename typeToCompString<T>::type>
    ::template append<decltype("\'\n"_compStr)>;
    static_assert(false,msg::sv);
};

template<typename T, typename C>
struct test_if_is_VSA_template_paremeter_pack_data_Impl<VSA_template_parameter_pack_data<T,C>>{
    using type = void;
};

}



// template<typename T,typename containerType>
// struct VSA_template_parameter_pack_data{
//     static constexpr size_t size = T::size;
//     template<size_t i>
//     static constexpr bool isType = detail::isTypeImpl<typename T::template at<i>>::value;
//     template<size_t i>
//     static constexpr bool isValue = !isType<i>;
//     template<size_t i>
//     using typeAt = typename detail::typeAtImpl<T,isType<i>,i>::type;
//     template<size_t i>
//     using typeStringAt = typename typeToCompString<typeAt<i>>::type;
//     template<size_t i>
//     static constexpr auto valueAt = detail::valueAtImpl<T,isValue<i>,i>::value;
//     template<size_t i>
//     using valueStringAt = typename valueToCompString<valueAt<i>>::type;
//     using container = containerType;
    
// };

template<typename T>
struct test_if_is_VSA_template_paremeter_pack_data{
    using type = detail::test_if_is_VSA_template_paremeter_pack_data_Impl<T>::type;
};

template<typename T>
struct VSA_translate_with_only_types;

template<template<typename...> typename container, typename... T>
struct VSA_translate_with_only_types<container<T...>>{
    using type = VSA_template_parameter_pack_data<detail::mp_list<T...>,container<void>>;
};

template<typename T>
struct VSA_translate_with_only_values;

template<template<auto...> typename container, auto... V>
struct VSA_translate_with_only_values<container<V...>>{
    using list = detail::mp_list<detail::valueAsType<V>...>;
    using type = VSA_template_parameter_pack_data<list,container<>>;
};


template<bool value, typename T, template<typename> typename msg, template<typename> typename translator>
struct verbose_static_assert_error_message_output{
    using errorMsg = typename msg<typename translator<T>::type>::msg;
    static_assert(value,errorMsg::sv);
    using type =void;
};

template<typename T, template<typename> typename msg, template<typename> typename translator>
struct verbose_static_assert_error_message_output<true,T,msg,translator>{
    using type = void;
};



template<typename T, template<typename> typename msg, template<typename> typename VSA_translater = VSA_translate_with_only_types>
struct verbose_static_assert{
    static constexpr auto value = T::value;
    //using assert_msg = typename msg<T>::msg;
    using init_static_assert = typename verbose_static_assert_error_message_output<value,T,msg,VSA_translater>::type;
};



}






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
    static_assert(str::pop_back<>::sv == "hello worl"sv);
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
    // reverse
    static_assert(str::reverse<>::sv == "dlrow olleh"sv);
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

        // const char*

        #define constCharPTests(n,x) static constexpr const char TTP_##n []= x; static_assert( vToSV<TTP_##n> == x""sv);

        constCharPTests(a,"hello world, abra, snore, galaxy world");
        constCharPTests(b,"23465134rsadf2gqraskfiq432t wmetrntq2u3nr v  m3ir 23trm w4it 20it imweiomt23mr i");
    };

    struct compStringConvTypes{
        template<typename T>
        static constexpr auto tToSv = typeToCompString<T>::type::sv;
        #define testType(x) static_assert(tToSv<x> == #x""sv,tToSv<x>);
        testType(void);
        testType(char);
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
        testType(bool(char,int));
        testType(int(char,int,double));
        testType(int(char,int,double) const);
        testType(int(char,int,double) const volatile);
        testType(int(char,int,double)&);
        testType(int(char,int,double)&&);
        testType(int(char,int,double) volatile&&);
        testType(int(*)(char,int,double));
        testType(const int(*)(char,int,double));
        testType(int(char,int,double) const&);
        testType(int(char,int,double) volatile&&);
        testType(const int(*&)(char,int,double));
        testType(const int(*&&)(char,int,double));
        testType(const double&(const char*,float&,bool));
        testType(const double&(const char*,float&,bool) const volatile&&);
        testType(const double&(*&&)(const char*,float&,bool));
        testType(int(* const&&)(char,int,double));
        testType(int(* const volatile&&)(char,int,double));
        testType(int(********)(char,int,double));
        testType(bool(* const&)(bool(*)(int),bool(*)(int)));
        testType(int(* const volatile* const* const&&)(char,int,double));
        testType(int(*)(char,int,double...));
    
    };



    // namespace typeToStringDefinitions{
    //         createTypeDefinition(some_none_fundamental_type);
    //     }

    
}

template<bool a, typename T, bool b, typename U>
struct myType{};

namespace compStringNS::compStringConvNS::templatedTypeDefinitions{

template<bool a, typename T, bool b, typename U>
struct templatedTypeToCompString<myType<a,T,b,U>>:std::true_type{
    using typeList = mp_list<valueAsType<a>,T,valueAsType<b>,U>;
    using typeName = decltype("myType"_compStr);
};
}

namespace{
    template<typename T>
    static constexpr auto tToSv = typeToCompString<T>::type::sv;
    #define testType(x) static_assert(tToSv<x> == #x""sv,tToSv<x>);
    struct TEST_template_types_toString{
        using t0 = myType<false,int,true,long int>;
        static_assert(tToSv<t0> == "myType<false,int,true,long int>"sv,tToSv<t0>);
    };
}





template<typename data, size_t i,typename ret>
struct are_all_same_VSA_message_impl{
    static constexpr auto f(){
        static_assert(std::is_same_v<typename data::template typeAt<0>,int>);


        if constexpr(data::size <=i){
            return ret{};
        }else{
            if constexpr(!std::is_same_v<typename data::template typeAt<0>,typename data::template typeAt<i>>){
            using namespace compStringNS::compStringConvNS;
            using msg = decltype("type "_compStr)::append<typename data::template typeStringAt<i>>
            ::template append<decltype(" != "_compStr)>::template append< typename data::template typeStringAt<0> >
            ::template append<decltype(" at pos "_compStr)>::template append<typename valueToCompString<i>::type >::template append<decltype(", \n"_compStr)>;
            using retT = ret::template append<msg>;
            using t = typename are_all_same_VSA_message_impl<data,i+1,retT>::type;
            return t{};
            }else{
                using t = typename are_all_same_VSA_message_impl<data,i+1,ret>::type;
                 return t{};
            }
        }
    }
    using type = decltype(f());
};

template<typename T>
struct are_all_same_VSA_message {
    verbose_static_assertNS::test_if_is_VSA_template_paremeter_pack_data<T> tester{};
    using msg = typename are_all_same_VSA_message_impl<T,1,decltype("\nFound differant types:\n"_compStr)>::type;
};

template<typename T,typename... U>
struct are_all_same{
    static constexpr bool value = ((std::is_same_v<T,U>)&&...);
};


template<typename typeData, size_t start,size_t i, typename ret>
struct are_all_differant_VSA_message_impl_check{
    static constexpr auto f(){
       
        if constexpr( i >= typeData::size){
            return ret{};
        }else
        if constexpr (std::is_same_v<typename typeData::template typeAt<start>,typename typeData::template typeAt<i>>){
            using namespace compStringNS::compStringConvNS;
            using typeName = typename typeData::template typeStringAt<start>;
            using appMsg = decltype("found same type "_compStr)::append<typeName>
            ::template append<decltype(" at pos "_compStr)>:: template append<typename valueToCompString<start>::type>
            ::template append<decltype(" and "_compStr)>:: template append<typename valueToCompString<i>::type>::template append<decltype(", \n"_compStr)>;
            using retT = ret::template append<appMsg>;
            return typename are_all_differant_VSA_message_impl_check<typeData,start,i+1,retT>::type{};
        }else{
            return typename are_all_differant_VSA_message_impl_check<typeData,start,i+1,ret>::type{};
        }
    }   
    using type = decltype(f());
};

template<typename typeData, typename typeSet, size_t i, typename ret>
struct are_all_differant_VSA_message_impl{
    static constexpr auto f(){
        if constexpr(i>=typeData::size){
            return ret{};
        }else
        if constexpr(! typeSet::template contains<typename typeData::template typeAt<i>>){
            using retT = typename are_all_differant_VSA_message_impl_check<typeData,i,i+1,ret>::type;
            using newTypeSet =typename typeSet::template add<typename typeData::template typeAt<i>>;
            return typename are_all_differant_VSA_message_impl<typeData,newTypeSet,i+1,retT>::type{};
        }else{
            return typename are_all_differant_VSA_message_impl<typeData,typeSet,i+1,ret>::type{};
        }
    }
    using type = decltype(f());
};

template<typename T>
struct SNBEI_VSA_message{
    static_assert(false);
    using msg = decltype("should not be instantiated"_compStr);
};

template<typename... T>
struct type_set{
    template<typename U>
    static constexpr bool contains = ((std::is_same_v<U,T>)||...);
    template<typename U>
    using add = type_set<T...,U>;
};

template<typename T>
struct are_all_differant_VSA_message{
    using typeSet = type_set<>;
    using msg = typename are_all_differant_VSA_message_impl<T,typeSet,0,decltype("\nSame types found:\n"_compStr)>::type;
};

template<typename T,typename...U>
struct are_all_differant{
    static constexpr bool value = !((std::is_same_v<T,U>)||...) && are_all_differant<U...>::value;
};

template<typename T>
struct are_all_differant<T>{
    static constexpr bool value = 1;
};


template<typename T,size_t indx, typename msg>
struct is_in_ascending_order_VSA_message_impl{
    static constexpr auto f(){
        if constexpr(indx >= T::size-1){
            return msg{};
        }else{
            constexpr auto v1 = T::template valueAt<indx>;
            constexpr auto v2 = T::template valueAt<indx+1>;
            if constexpr(v1 >= v2){
            using apMsg = decltype("At pos "_compStr)
            ::template append<typename valueToCompString<indx>::type>
            ::template append<decltype(" and "_compStr)>
            ::template append<typename valueToCompString<indx+1>::type>
            ::template append<decltype(", the value "_compStr)>
            ::template append<typename valueToCompString<v1>::type>
            ::template append<decltype(" !< "_compStr)>
            ::template append<typename valueToCompString<v2>::type>
            ::template append<decltype(", types are: \'"_compStr)>
            ::template append<typename typeToCompString<decltype(v1)>::type>
            ::template append<decltype("\' and \'"_compStr)>
            ::template append<typename typeToCompString<decltype(v2)>::type>
            ::template append<decltype("\',\n"_compStr)>;
            using rmsg = typename msg::template append<apMsg>;
            return typename is_in_ascending_order_VSA_message_impl<T,indx+1,rmsg>::type{};
            }else{
                return typename is_in_ascending_order_VSA_message_impl<T,indx+1,msg>::type{};
            }

        }
    }
    using type = decltype(f());
};

template<typename T>
struct is_in_ascending_order_VSA_message{
    using msg = typename is_in_ascending_order_VSA_message_impl<T,0,decltype("\nFound values in non ascending order:\n"_compStr)>::type;
    //using msg = decltype("eroor msg"_compStr);
};


template<auto... Vs>
struct is_in_ascending_order{
    static constexpr bool value = true;
};

template<auto V, auto U, auto... other>
struct is_in_ascending_order<V,U,other...>{
    static constexpr bool value = (V < U) && is_in_ascending_order<U,other...>::value;
};

template<auto V, auto U>
struct is_in_ascending_order<V,U>{
    static constexpr bool value = (V < U);
};

template<auto V>
struct is_in_ascending_order<V>{
    static constexpr bool value = 1;
};


template<typename>
struct translaterT;

//create the specialization for the pattern of we want
template<template<typename,bool,typename...> typename container, typename T, bool A, typename... Us>
struct translaterT<container<T,A,Us...>>{
	//here we will create a mp_list of the template parameters
	using list = verbose_static_assertNS::detail::mp_list< T, verbose_static_assertNS::detail::valueAsType<A>, Us... >;
	// Note that we converted the value A into the wrapper type 'valueAsType'
	// and that all parameters in mp_list are in the same order as MpFunc
	
	//container type
	//here we pass the type container with some arbitrary parameters.
	using contT = container<void,0>;

	//create the final VSA_template_parameter_pack_data type
	using type = verbose_static_assertNS::VSA_template_parameter_pack_data<list,contT>;
};

//A helper type the will be called recursively to iterate over the types of Us and generate the error message
template<typename T, size_t i, typename msg>
// Here T is the VSA_template_parameter_pack_data, i is the index of the current tested type, and msg is the previous part of the message that we will append to
struct MpFunc_VSA_message_impl{
    static constexpr auto f(){
        // Here we will break recursion after we went through all the types
        if constexpr (T::size <= i){
            //went through all types, finish message,
            //depending on bool A appen an endline symbol to the end
            if constexpr(T::template valueAt<1> == 1){
                using retMsg = typename msg::template append<decltype("\n"_compStr)>;// adding "/n" to the end of message
                return retMsg{};
            }else{
                return msg{}; // returning message as is
            }
        }else{
            // testing type at i
            constexpr bool A = T::template valueAt<1>; // value of bool A
            using baseT = typename T::template typeAt<0>; // type T
            using otherT = typename T::template typeAt<i>; // type Us[i]
            // if A == true and T == Us[i], then upadate the message
            if constexpr(A&&std::is_same_v<baseT,otherT>){ 
                using addOnMessage = typename valueToCompString<i>::type::template append<decltype(", "_compStr)>; // string to add to message
                using retMsg = typename msg::template append<addOnMessage>; // appending 
                return typename MpFunc_VSA_message_impl<T,i+1,retMsg>::type{}; // recursive call, with the updated message
            }else
            // if A == false and T != Us[i], then update the message
            if constexpr((!A)&&(!std::is_same_v<baseT,otherT>)){
                using addOnMessage = typename decltype("the type '"_compStr)
                ::template append<typename T::template typeStringAt<i>>
                ::template append<decltype("' at pos: "_compStr)>
                ::template append<typename valueToCompString<i>::type>
                ::template append<decltype("\n"_compStr)>; // string to add to message
                using retMsg = typename msg::template append<addOnMessage>;
                return typename MpFunc_VSA_message_impl<T,i+1,retMsg>::type{};// recursive call, with the updated message
            }else{
                // no error state achived, so move on to the next type, with message unchanged 
                return typename MpFunc_VSA_message_impl<T,i+1,msg>::type{};// recursive call, with the mesage as is
            }
        }
    };
    using type = decltype(f()); // return type of f() is a compString containing the message
};

//the message template type
template<typename T>
struct MpFunc_VSA_message{
    // depending on bool A, the message will be began differently 
	static constexpr auto f(){
		if constexpr(T::template valueAt<1> == 1){
			using retMsg = typename decltype("\nfound types that are equal to the type '"_compStr)	
					::template append<typename T::template typeStringAt<0> >
					::template append<decltype("' at pos:\n"_compStr)>;
			return retMsg{}; // bool A == true
		}else{
			using retMsg = typename decltype("\nfound types that are differnt from the type '"_compStr)	
					::template append<typename T::template typeStringAt<0> >
					::template append<decltype("':\n"_compStr)>;
			return retMsg{}; // bool A == false
		}
	}
	
	using msg = typename MpFunc_VSA_message_impl<T,2,decltype(f())>::type; // call the recursive type MpFunc_VSA_message_impl to generate the error message
};

template<typename T, bool A, typename... Us>
struct MpFunc{
    static constexpr bool value = false;
};



template<template<typename,typename> typename comparitor,typename A, typename B>
struct test_comparitor_ab{

    template<template<typename,typename> typename D>
    static bool test(D<A,B>*);
    template<template<typename,typename> typename D>
    static void test(...);

    static constexpr bool value = std::is_same_v<bool,decltype(test<comparitor>(0))>;
};


template<template<typename,typename> typename comparitor,typename T, typename U, typename...Us> 
struct is_comparitor_for_template_pack_column{
    static constexpr bool f(){
        if constexpr(sizeof...(Us) == 0){
            return test_comparitor_ab<comparitor,T,U>::value;
        }else{
            return test_comparitor_ab<comparitor,T,U>::value && is_comparitor_for_template_pack_column<comparitor,T,Us...>::value;
        }
    }
    static constexpr bool value = f();
};

template<template<typename,typename> typename comparitor,typename T, typename...Us> 
struct is_comparitor_for_template_pack{
    static constexpr bool f(){
        if constexpr(sizeof...(Us)==0){
            return true;
        }else{
            return is_comparitor_for_template_pack_column<comparitor,T,Us...>::value && is_comparitor_for_template_pack<comparitor,Us...>::value;
        }
    }
    static constexpr bool value = f();
};

template<typename T, typename comparitor, size_t indx, size_t offset, typename msg>
struct is_comparitor_for_template_pack_VSA_message_line_impl;

template<typename T, size_t indx, size_t offset, typename msg, template<typename,typename> typename comparitor>
struct is_comparitor_for_template_pack_VSA_message_line_impl<T,comparitor<char,char>,indx,offset,msg>{
    static constexpr auto f(){
        if constexpr(offset > T::size-1){
            return msg{};
        }else{
            if constexpr(!test_comparitor_ab<comparitor,typename T::typeAt<indx>, typename T::typeAt<offset>>::value){
                using addOnMessage = typename decltype("could not create the comparitor type '"_compStr)
                ::template append<typename typeToCompString<comparitor<int,int>>::template templateName<>>
                ::template append<decltype("' with the types '"_compStr)>
                ::template append<typename typeToCompString<typename T::typeAt<indx>>::type>
                ::template append< decltype("' and '"_compStr)>
                ::template append< typename typeToCompString<typename T::typeAt<offset>>::type >
                ::template append<decltype("', at positions "_compStr)>
                ::template append<typename valueToCompString<indx>::type>
                ::template append<decltype(" and "_compStr)>
                ::template append<typename valueToCompString<offset>::type>
                ::template append<decltype("\n"_compStr)>;
                using retMsg = typename msg::template append<addOnMessage>;
                return typename is_comparitor_for_template_pack_VSA_message_line_impl<T,comparitor<char,char>,indx,offset+1,retMsg>::type{};
            }else{
                return typename is_comparitor_for_template_pack_VSA_message_line_impl<T,comparitor<char,char>,indx,offset+1,msg>::type{};
            }
        }
    }
    using type = decltype(f());
};

template<typename T, size_t indx, typename msg>
struct is_comparitor_for_template_pack_VSA_message_impl{
    static constexpr auto f(){
        if constexpr( indx > T::size-1){
            return msg{};
        }else{
            using newMsg =  typename is_comparitor_for_template_pack_VSA_message_line_impl<T,typename T::typeAt<0>, indx,indx+1,msg>::type;
            return typename is_comparitor_for_template_pack_VSA_message_impl<T,indx+1,newMsg>::type{};
        }
    }
    using type = decltype(f());
};

template<typename T>
struct is_comparitor_for_template_pack_VSA_message{
    using msg = typename is_comparitor_for_template_pack_VSA_message_impl<T,1,decltype("\nfailer to creat comparitor:\n"_compStr)>::type;
};

template<typename T>
struct is_comparitor_for_template_pack_VSA_translate;

template<template<typename,typename> typename comparitor,typename T, typename... Us>
struct is_comparitor_for_template_pack_VSA_translate<is_comparitor_for_template_pack<comparitor,T,Us...>>{
    using type = verbose_static_assertNS::VSA_template_parameter_pack_data<verbose_static_assertNS::detail::mp_list<comparitor<char,char>,T,Us...>,void>;
};


template<typename A,typename B>
concept is_comparibale = requires(A a, B b){
    {a < b};
    {b < a};
};

struct some_none_fundamental_type{
    char v[10]; 
};

template<typename A, typename B>
requires is_comparibale<A,B> || (std::is_same_v<A,int> && std::is_same_v<B,some_none_fundamental_type>)
struct myComparitor{
    bool operator()(A a, B b);
};

namespace compStringNS{
namespace compStringConvNS{
        namespace typeToStringDefinitions{
            createTypeDefinition(some_none_fundamental_type);
        }

        namespace templatedTypeDefinitions{
            template< typename A, typename B>
            struct templatedTypeToCompString<myComparitor<A,B>>:std::true_type{
            using typeList = mp_list<A,B>;
            using typeName = decltype("myComparitor"_compStr);
            };
        }     
}
}

int main() {
 
    using namespace verbose_static_assertNS;
   
    using t = are_all_same<int,int,char,float,int,double,int,long int,int>;
    verbose_static_assert<t,are_all_same_VSA_message>{};
    using y = are_all_differant<int,double&&,const char* const volatile&,long int,int, char,float,double&,double&&,char,double&,const char* const volatile&>;
    verbose_static_assert<y,are_all_differant_VSA_message>{};
    verbose_static_assert<std::is_same<int,int>,SNBEI_VSA_message>{};
    using z = is_in_ascending_order<1,5u,3,4,'4',6,'\\',-1,-6>;
    verbose_static_assert<z,is_in_ascending_order_VSA_message,VSA_translate_with_only_values>{};
    using for_VSA_a = MpFunc<int,1,bool,char,int,long int, double,int,int,char,int>;
    verbose_static_assert<for_VSA_a,MpFunc_VSA_message,translaterT>{};
    using for_VSA_b = MpFunc<int,0,bool,char,int,long int, double,int,int,char,int>;
    verbose_static_assert<for_VSA_b,MpFunc_VSA_message,translaterT>{};
    using w = is_comparitor_for_template_pack<myComparitor,int,long int,some_none_fundamental_type,char>;
    verbose_static_assert<w,is_comparitor_for_template_pack_VSA_message,is_comparitor_for_template_pack_VSA_translate>{};
    return 0;

    static constexpr char ptr[] = "elvis curl";
    constexpr auto sptr = valueToCompString<ptr>::type::sv;
    static_assert(sptr == "elvis curl"sv);
    constexpr auto sptr2  = charArrayToType<ptr,5>::type::sv;
    static_assert(sptr2 == "elvis"sv);

    using ftype = bool(int) &&;
    static_assert(std::is_function_v<ftype>);
    static_assert(!std::is_reference_v<ftype>);
    static_assert(std::is_same_v<std::remove_reference_t<ftype>,bool(int) &&>);
}