#include "compileTime String.hpp"


namespace verbose_static_assertNS{
using namespace compStringNS;
using namespace compStringNS::compStringConvNS;
namespace detail{
template<size_t i,size_t target,typename T,typename... Ts>
struct mp_list_at_impl{
    static constexpr auto f(){
        if constexpr(sizeof...(Ts) == 0){
            return T{};
        }else
        if constexpr(i == target){
            return T{};
        }else{
            return typename mp_list_at_impl<i+1,target,Ts...>::type{};
        }
    }
    using type = decltype(f());
};

template<typename... T>
struct mp_list{
    template<size_t i>
    using at =typename mp_list_at_impl<0,i,T...>::type;
};
}

template<typename T>
struct verbose_static_assert_messageData;

template<template<typename...> typename caller,typename...T>
struct verbose_static_assert_messageData<caller<T...>>{
    static constexpr size_t argSize = sizeof...(T);
    template<typename...Ts>
    using templateType = caller<Ts...>;
    using templateTypeString = typename templateNames<caller>::name;
    template<size_t i>
    using typeAt = typename detail::mp_list<T...>::at<i>;
    template<size_t i>
    using typeStringAt = typename typeToCompString<typeAt<i>>::type;
};

template<typename T,template<typename...> typename msg>
struct verbose_static_assert;

template<template<typename...> typename caller,typename...T, template<typename...> typename msg>
struct verbose_static_assert<caller<T...>,msg>{
    using ExprT = caller<T...>;
    static constexpr auto value = ExprT::value;

     using name = typename templateNames<caller>::name;
    static constexpr size_t argCount = sizeof...(T);
    using argList = detail::mp_list<T...>;
    using assert_msg = typename msg<caller<T...>>::msg;
    static_assert(value,assert_msg::sv);

    static void f(){
        std::cout<<"msg: "<<assert_msg::sv<<std::endl;
        std::cout<<"name: "<<name::sv<<std::endl;
        int i=0;
        ((std::cout<<"arg["<<i++<<"] "<<typeToCompString<T>::type::sv<<std::endl),...);
        std::cout<<"TET:  "<<typeToCompString< typename argList::at<2>  >::type::sv<<std::endl;
        std::cout<<"value: "<<value<<std::endl;
    }
};
}