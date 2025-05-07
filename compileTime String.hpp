#include <string_view>
#include <type_traits>



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
struct compString;

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
       using strA = A::template substr<(pos1 == -1 ? 0 : pos1),(pos1 == -1 ? A::size : pos1 + count1)>;
       using strB = B::template substr<(pos2 == -1 ? 0 : pos2),(pos2 == -1 ? B::size : pos2 + count2)>;
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

};

template <typename T, T... chars>
constexpr compString<detail::tstring<chars...>> operator""_compStr() {
  return {};
}

}