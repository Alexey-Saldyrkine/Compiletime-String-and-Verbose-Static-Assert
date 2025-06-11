#pragma once
#include "compString.hpp"
#include "compString_conversion.hpp"


namespace verbose_static_assertNS{
using namespace compStringNS;
using namespace compStringNS::compStringConvNS;

template<typename T,typename containerType = void>
using VSA_template_parameter_pack_data = detailPPD::detail_TPPD<T,containerType>;

namespace detail{

using namespace compStringNS::compStringConvNS::detailPPD;

template<typename T>
struct test_if_is_VSA_template_paremeter_pack_data_Impl{
    using msg = typename decltype("given type is not a VAS_tempalte_parameter_pack_data type, it is the type \'"_compStr)
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
    using type = typename detail::test_if_is_VSA_template_paremeter_pack_data_Impl<T>::type;
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