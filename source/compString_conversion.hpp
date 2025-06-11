#pragma once
#include "compString.hpp"

namespace compStringNS{
namespace compStringConvNS{



namespace typeToStringDefinitions{

// template<typename T>
// int render();

template<typename T>
struct typeToCompStringInter{
    using type = decltype("[no name given to type]"_compStr);
    //static constexpr auto i = render<T>();
};

#define createTypeDefinition(x) template<> struct typeToCompStringInter<x>{ using type = decltype(#x ""_compStr);};

template<> struct typeToCompStringInter<void>{ using type = decltype("void"_compStr);};

//createTypeDefinition(void);
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
    static constexpr decltype(val) value = val;
};

template<typename T>
struct templateTemplateType{
    using type = T;
};

template<typename T>
struct isWhatParameter{
    static constexpr bool isType = true;
    static constexpr bool isValue = false;
    static constexpr bool isTemplate = false;
};
template<auto val>
struct isWhatParameter<valueAsType<val>>{
    static constexpr bool isType = false;
    static constexpr bool isValue = true;
    static constexpr bool isTemplate = false;
};

template<typename T>
struct isWhatParameter<templateTemplateType<T>>{
    static constexpr bool isType = false;
    static constexpr bool isValue = false;
    static constexpr bool isTemplate = true;
};

template<bool falseV,typename T, size_t i, bool isValue>
struct typeAtImplFailerMsg{
    using type = typename T::template at<i>;
};

template<typename T, size_t i>
struct typeAtImplFailerMsg<false,T,i,true>{
    #if __cpp_static_assert >= 202306L
    using vWrap = typename T::template at<i>;
    using msg = typename decltype("VSA_TPP_data::typeAt - tried to access template parameter at pos "_compStr)
    ::template append<typename valueToCompString<i>::type>
    ::template append<decltype(" is not a type, but a vlaue of type \'"_compStr)>
    ::template append<typename typeToCompString<typename vWrap::type>::type>
    ::template append<decltype("\' and eqaul to ")>
    ::template append<typename valueToCompString<vWrap::value>::type>
    ::template append<decltype("\n"_compStr)>;

    static_assert(false,msg::sv);
    #else
    static_assert(false,"typeAt: not a type, but a value");
    #endif
};

template<typename T, size_t i>
struct typeAtImplFailerMsg<false,T,i,false>{
    #if __cpp_static_assert >= 202306L
    using vWrap = typename T::template at<i>;
    using msg = decltype("VSA_TPP_data::typeAt - tried to access template parameter at pos "_compStr)
    ::template append<typename valueToCompString<i>::type>
    ::template append<decltype(" is not a type, but a template template parameter of \'"_compStr)>
    ::template append<typename typeToCompString<typename vWrap::type>::template templateName<>>
    ::template append<decltype("\'\n"_compStr)>;

    static_assert(false,msg::sv);
    #else
    static_assert(false,"typeAt: not a type, but a template template parameter");
    #endif
};


template<typename T,bool isType, size_t indx, bool isValue>
struct typeAtImpl{
    using type = typename typeAtImplFailerMsg<isType,T,indx, isValue>::type;
};

template<bool falseV,typename T,size_t i, bool isType>
struct valueAtImplFailerMsg{
    static constexpr auto value = T::template at<i>::value;
};

template<typename T, size_t i>
struct valueAtImplFailerMsg<false,T,i,true>{
    #if __cpp_static_assert >= 202306L
    using msg = decltype("VSA_TPP_data::valueAt -  tried to access template parameter at pos "_compStr)
    ::template append<typename valueToCompString<i>::type>
    ::template append<decltype(" is not a value, but a type \'"_compStr)>
    ::template append<typename typeToCompString<typename T::template at<i>>::type>
    ::template append<decltype("\'\n"_compStr)>;

    static_assert(false,msg::sv);
    #else
    static_assert(false,"valueAt: not a value, but a type");
    #endif
};

template<typename T, size_t i>
struct valueAtImplFailerMsg<false,T,i,false>{
    #if __cpp_static_assert >= 202306L
    using vWrap = typename T::template at<i>;
    using msg = decltype("VSA_TPP_data::valueAt -  tried to access template parameter at pos "_compStr)
    ::template append<typename valueToCompString<i>::type>
    ::template append<decltype(" is not a value, but a template template parameter of \'"_compStr)>
    ::template append<typename typeToCompString<typename vWrap::type>::template templateName<>>
    ::template append<decltype("\'\n"_compStr)>;

    static_assert(false,msg::sv);
    #else
    static_assert(false,"valueAt: not a value, but a template template parameter");
    #endif
};

template<typename T, bool isValue, size_t indx, bool isType>
struct valueAtImpl{
    static constexpr auto value = valueAtImplFailerMsg<isValue,T,indx,isType>::value;
};

template<bool isTemplate,typename T, size_t indx, bool isType>
struct parmaterAtImplFailerMsg{
    using vWrap = typename T::template at<indx>;
    using type = typename typeToCompString<typename vWrap::type>::template templateName<>;
};

template<typename T, size_t i>
struct parmaterAtImplFailerMsg<false,T,i,true>{
    #if __cpp_static_assert >= 202306L
    using msg = decltype("VSA_TPP_data::templateAt -  tried to access template parameter at pos "_compStr)
    ::template append<typename valueToCompString<i>::type>
    ::template append<decltype(" is not a template template parameter, but a type \'"_compStr)>
    ::template append<typename typeToCompString<typename T::template at<i>>::type>
    ::template append<decltype("\'\n"_compStr)>;

    static_assert(false,msg::sv);
    #else
    static_assert(false,"templateAt: not a template template parameter, but a type");
    #endif
};

template<typename T, size_t i>
struct parmaterAtImplFailerMsg<false,T,i,false>{
    #if __cpp_static_assert >= 202306L
    using vWrap = typename T::template at<i>;
    using msg = decltype("VSA_TPP_data::templateAt -  tried to access template parameter at pos "_compStr)
    ::template append<typename valueToCompString<i>::type>
    ::template append<decltype(" is not a template template parameter, but a vlaue of type \'"_compStr)>
    ::template append<typename typeToCompString<typename vWrap::type>::type>
    ::template append<decltype("\' and eqaul to ")>
    ::template append<typename valueToCompString<vWrap::value>::type>
    ::template append<decltype("\n"_compStr)>;

    static_assert(false,msg::sv);
    #else
    static_assert(false,"templateAt: not a template template parameter, but a value");
    #endif
};

template<typename T, bool isTemplate, size_t indx, bool isType>
struct parmaterAtImpl{
    using type = typename parmaterAtImplFailerMsg<isTemplate,T,indx,isType>::type;
};

template<bool isValue,bool isParameter, typename T, size_t i>
struct stringAt_impl{
    static constexpr auto f(){
        if constexpr(isParameter){
            return typename parmaterAtImpl<T,true,i,false>::type{};
        }else
        if constexpr(isValue){
            return typename valueToCompString<valueAtImpl<T,true,i,false>::value>::type{};
        }else{
            return typename typeToCompString<typename typeAtImpl<T,true,i,false>::type>::type{}; 
        }
    }

    using type = decltype(f());
};

template<typename T,typename containerType>
struct detail_TPPD{
    static constexpr size_t size = T::size;
    template<size_t i>
    static constexpr bool isType = isWhatParameter<typename T::template at<i>>::isType;
    template<size_t i>
    static constexpr bool isValue = isWhatParameter<typename T::template at<i>>::isValue;
    template<size_t i>
    static constexpr bool isTemplate = isWhatParameter<typename T::template at<i>>::isTemplate;
    template<size_t i>
    using typeAt = typename typeAtImpl<T,isType<i>,i,isValue<i>>::type;
    template<size_t i>
    using typeStringAt = typename typeToCompString<typeAt<i>>::type;
    template<size_t i>
    static constexpr auto valueAt = valueAtImpl<T,isValue<i>,i,isType<i>>::value;
    template<size_t i>
    using valueStringAt = typename valueToCompString<valueAt<i>>::type;
    template<size_t i>
    using templateAt = typename parmaterAtImpl<T,isTemplate<i>,i,isType<i>>::type;
    template<size_t i>
    using stringAt = typename stringAt_impl<isValue<i>,isTemplate<i>,T,i>::type;
    using container = containerType;
};



}


namespace templatedTypeDefinitions{
using detailPPD::valueAsType;
using detailPPD::templateTemplateType;
using detailPPD::mp_list;




template<typename T>
struct templatedTypeToCompString{
    using typeList = void; // no list means to template
    using typeName = decltype("not a defiend template type"_compStr);
   
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
        using str = typename typeToCompString<T>::type;
        using newMsg = typename msg::template append<str>;
        if constexpr(sizeof...(Ts) != 0){
            using retMsg = typename newMsg::template append<decltype(", "_compStr)>;
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
    using type = typename T::template pop_back<>::template append<decltype(",...)"_compStr)>;
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
    using type = typename base::template append<decltype(" const"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) volatile>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = typename base::template append<decltype(" volatile"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) const volatile>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = typename base::template append<decltype(" const volatile"_compStr)>;
};

// basic function ref
// &
template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) &>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = typename base::template append<decltype("&"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) const &>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = typename base::template append<decltype(" const&"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) volatile &>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = typename base::template append<decltype(" volatile&"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) const volatile &>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = typename base::template append<decltype(" const volatile&"_compStr)>;
};
// &&

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) &&>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = typename base::template append<decltype("&&"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) const &&>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = typename base::template append<decltype(" const&&"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) volatile&&>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = typename base::template append<decltype(" volatile&&"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) const volatile&&>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = typename base::template append<decltype(" const volatile&&"_compStr)>;
};

// all above but variadic function

// basic function cv
template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...,...)>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = base;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...,...) const>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = typename base::template append<decltype(" const"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...,...) volatile>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = typename base::template append<decltype(" volatile"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...,...) const volatile>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = typename base::template append<decltype(" const volatile"_compStr)>;
};

// basic function ref
// &
template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...,...) &>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = typename base::template append<decltype("&"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...,...) const &>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = typename base::template append<decltype(" const&"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...,...) volatile &>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = typename base::template append<decltype(" volatile&"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...,...) const volatile &>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = typename base::template append<decltype(" const volatile&"_compStr)>;
};
// &&

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...,...) &&>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = typename base::template append<decltype("&&"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...,...) const &&>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = typename base::template append<decltype(" const&&"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...,...) volatile&&>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = typename base::template append<decltype(" volatile&&"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...,...) const volatile&&>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = typename base::template append<decltype(" const volatile&&"_compStr)>;
};


// all above but noexcept
// basic function cv
template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) noexcept>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = typename base::template append<decltype(" noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) const noexcept>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = typename base::template append<decltype(" const noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) volatile noexcept>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = typename base::template append<decltype(" volatile noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) const volatile noexcept>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = typename base::template append<decltype(" const volatile noexcept"_compStr)>;
};

// basic function ref
// &
template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) & noexcept>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = typename base::template append<decltype("& noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) const & noexcept>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = typename base::template append<decltype(" const& noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) volatile & noexcept>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = typename base::template append<decltype(" volatile& noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) const volatile & noexcept>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = typename base::template append<decltype(" const volatile& noexcept"_compStr)>;
};
// &&

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) && noexcept>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = typename base::template append<decltype("&& noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) const && noexcept>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = typename base::template append<decltype(" const&& noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) volatile&& noexcept>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = typename base::template append<decltype(" volatile&& noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...) const volatile&& noexcept>{
    using base = typename functionBase<Ret,Args...>::type;
    using type = typename base::template append<decltype(" const volatile&& noexcept"_compStr)>;
};

// all above but variadic function

// basic function cv
template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...,...) noexcept>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = typename base::template append<decltype(" noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...,...) const noexcept>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = typename base::template append<decltype(" const noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...,...) volatile noexcept>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = typename base::template append<decltype(" volatile noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...,...) const volatile noexcept>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = typename base::template append<decltype(" const volatile noexcept"_compStr)>;
};

// basic function ref
// &
template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...,...) & noexcept>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = typename base::template append<decltype("& noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...,...) const & noexcept>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = typename base::template append<decltype(" const& noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...,...) volatile & noexcept>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = typename base::template append<decltype(" volatile& noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...,...) const volatile & noexcept>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = typename base::template append<decltype(" const volatile& noexcept"_compStr)>;
};
// &&

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...,...) && noexcept>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = typename base::template append<decltype("&& noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...,...) const && noexcept>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = typename base::template append<decltype(" const&& noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...,...) volatile&& noexcept>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = typename base::template append<decltype(" volatile&& noexcept"_compStr)>;
};

template<typename Ret, typename... Args>
struct typeToCompString_function_type<Ret(Args...,...) const volatile&& noexcept>{
    using base = typename functionBaseToVariadic<typename functionBase<Ret,Args...>::type>::type;
    using type = typename base::template append<decltype(" const volatile&& noexcept"_compStr)>;
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
        using t = typename str::template push_back<'0'+i>;
        return typename integerValToCompString<T,val/10,t>::type{};
        }else{
             using t = typename str::template push_back<static_cast<char>('0'+val)>;
             return t{};
        }
    }

    using type = decltype(f());
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

template<typename T>
struct req_remove_ref_pointer{
    static constexpr bool isMemberPtr = false;
    using type = T;
};

template<typename T>
struct req_remove_ref_pointer<T&>{
    using type = typename req_remove_ref_pointer<T>::type;
    static constexpr bool isMemberPtr = req_remove_ref_pointer<T>::isMemberPtr;
};

template<typename T>
struct req_remove_ref_pointer<T&&>{
    using type = typename req_remove_ref_pointer<T>::type;
    static constexpr bool isMemberPtr = req_remove_ref_pointer<T>::isMemberPtr;
};

template<typename T>
struct req_remove_ref_pointer<T*>{
    using type = typename req_remove_ref_pointer<T>::type;
    static constexpr bool isMemberPtr = req_remove_ref_pointer<T>::isMemberPtr;
};

template<typename T>
struct req_remove_ref_pointer<T* const>{
    using type = typename req_remove_ref_pointer<T>::type;
    static constexpr bool isMemberPtr = req_remove_ref_pointer<T>::isMemberPtr;
};

template<typename T>
struct req_remove_ref_pointer<T* volatile>{
    using type = typename req_remove_ref_pointer<T>::type;
    static constexpr bool isMemberPtr = req_remove_ref_pointer<T>::isMemberPtr;
};

template<typename T>
struct req_remove_ref_pointer<T* const volatile>{
    using type = typename req_remove_ref_pointer<T>::type;
    static constexpr bool isMemberPtr = req_remove_ref_pointer<T>::isMemberPtr;
};

template<typename T, typename U>
struct req_remove_ref_pointer<T U::*>{
    using type = T U::*;
    static constexpr bool isMemberPtr = true;
};


template<typename T>
struct typeToCompString_is_base_a_function{
    using type = typename req_remove_ref_pointer<T>::type;
    static constexpr bool isMemberPtr = req_remove_ref_pointer<T>::isMemberPtr;
    static constexpr bool value = std::is_function_v<type> ;
};

template<typename T>
struct typeToString_array;

template<typename T>
struct typeToString_array<T[]>{
    using type =typename typeToCompString<T>::type::template append<decltype("[]"_compStr)>;
};

template<typename T, size_t N>
struct typeToString_array<T[N]>{
    using type =typename typeToCompString<T>::type::template append<decltype("["_compStr)>::template append<typename valueToCompString<N>::type>::template append<decltype("]"_compStr)>;
};

template<typename T, bool funcOverride>
struct typeOrTemplateToCompString;

template<typename T, bool>
struct typeToString_memberPointer;

template<typename T, typename U, bool bv>
struct typeToString_memberPointer<T U::*,bv>{
    static constexpr auto f(){
        if constexpr(std::is_member_function_pointer_v<T U::*>){
             using Tstr0 = typename typeOrTemplateToCompString<T,bv>::type;
             constexpr size_t pos0 = Tstr0::template find<decltype("("_compStr)>-1;
             using Tstr = typename Tstr0::template insert<pos0,decltype("(*)"_compStr)>;
             using Ustr = typename typeOrTemplateToCompString<U,bv>::type::template append<decltype("::"_compStr)>;
             constexpr size_t pos = Tstr::template find<decltype("*"_compStr)> -1;
             using retStr = typename Tstr::template insert<pos,Ustr>;
             return retStr{};
        }else{
            using Tstr = typename typeOrTemplateToCompString<T*,bv>::type;
            using Ustr = typename typeOrTemplateToCompString<U,bv>::type::template append<decltype("::"_compStr)>::template prepend<decltype(" "_compStr)>;
            constexpr size_t pos = Tstr::template find<decltype("*"_compStr)> -1;
            using retStr = typename Tstr::template insert<pos,Ustr>;
            return retStr{};
        }
    }
    
    using type = decltype(f());
};

template<typename>
struct removeMemberPtr;

template<typename T, typename U>
struct removeMemberPtr<T U::*>{
    using type = T;
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
            if constexpr(!funcSigOverride && detail::typeToCompString_is_base_a_function<T>::isMemberPtr ){

                 using ptrStr = typename typeOrTemplateToCompString<T,true>::type;
                 using memberPtrT = typename detail::typeToCompString_is_base_a_function<T>::type;
                 using baseType = typename removeMemberPtr<memberPtrT>::type;
                 using baseStr = typename typeToString_memberPointer<memberPtrT,false>::type;
                 if constexpr(! ptrStr::template equal<decltype(""_compStr)>){
                    constexpr size_t pos0 = baseStr::template find<decltype("("_compStr)>;
                    constexpr size_t pos = baseStr::template find<decltype("*"_compStr),pos0>;
                    using retStr = typename baseStr::template insert<pos,ptrStr>;
                    return retStr{};
                }else{
                    return baseStr{};
                }
                 //return ptrStr{};
            }else
            if constexpr(!funcSigOverride && detail::typeToCompString_is_base_a_function<T>::value){
                using ptrStr = typename typeOrTemplateToCompString<T,true>::type;
                using baseFunc = typename detail::typeToCompString_is_base_a_function<T>::type;
                using funcStr = typename detail::typeToCompString_function_type<baseFunc>::type;
                if constexpr(! ptrStr::template equal<decltype(""_compStr)>){
                    using fptr = typename decltype("("_compStr):: append<ptrStr>::template append<decltype(")"_compStr)>;
                    constexpr size_t pos = funcStr::template find<decltype("("_compStr)>-1;
                    using retStr = typename funcStr::template insert<pos,fptr>;
                    return retStr{};
                }else{
                    return funcStr{};
                }
            }else
            if constexpr(funcSigOverride && std::is_member_pointer_v<T>){
                return decltype(""_compStr){};
            }else
           if constexpr(funcSigOverride && std::is_function_v<T>){
                return decltype(""_compStr){};
                //return typename detail::typeToCompString_function_type<T>::type{};
           }else
           if constexpr(std::is_reference_v<T>){
                using t = std::remove_reference_t<T>;
                if constexpr(std::is_lvalue_reference_v<T>){
                    return typename typeOrTemplateToCompString<t,funcSigOverride>::type::template append<decltype("&"_compStr)>{};
                }else{
                    return typename typeOrTemplateToCompString<t,funcSigOverride>::type::template append<decltype("&&"_compStr)>{};
                }
           }else
           if constexpr(std::is_const_v<T> || std::is_volatile_v<T>){
                using cvStr = typename detail::typeToCompString_cv<T>::type;
                using t = std::remove_cv_t<T>;
                if constexpr(std::is_pointer_v<T>){
                    return typename typeOrTemplateToCompString<t,funcSigOverride>::type::template append<decltype(" "_compStr)>::template append<cvStr>{};
                }else{
                    using retStr = typename cvStr::template append<decltype(" "_compStr)>::template append<typename typeOrTemplateToCompString<t,funcSigOverride>::type>;
                    return retStr{};
                }
           }else
           if constexpr(std::is_pointer_v<T>){
                using t = std::remove_pointer_t<T>;
                return typename typeOrTemplateToCompString<t,funcSigOverride>::type::template append<decltype("*"_compStr)>{};
                
           }else{   
                    if constexpr(std::is_array_v<T>){
                        return typename typeToString_array<T>::type{};
                    }else{
                    // fundamental and class types
                    return typename typeToStringDefinitions::typeToCompStringInter<T>::type{};
                    }
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
            using retmsg = typename msg::template append<typename Data::template stringAt<i>>;
            return retmsg{};
        }else{
            using retmsg = typename msg::template append<typename Data::template stringAt<i>>::template append<decltype(", "_compStr)>;
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

    using type = typename templateName::template append<decltype("<"_compStr)>::template append<parameterStr>::template append<decltype(">"_compStr)>;
};



template<typename T, bool funcOverride>
struct typeOrTemplateToCompString{
    static constexpr auto f(){
        if constexpr(!std::is_same_v<typename templatedTypeDefinitions::templatedTypeToCompString<T>::typeList,void>){
            return typename templatedTypeToCompString<T>::type{};
        }else{
           
            return typename detail::typeToCompStringInterm<T,funcOverride>::type{};
        }
    }

    using type = decltype(f());
};

}


template<typename T>
struct typeToCompString{
    using type = typename detail::typeOrTemplateToCompString<T,false>::type;

    template<typename = void>
    using templateName = typename templatedTypeDefinitions::templatedTypeToCompString<T>::typeName;
};



template<typename T, auto val>
struct valueToCompStringInter{
    using type = typename decltype("[no value to compString conversion for the type '"_compStr)::append<typename typeToCompString<T>::type>::template append<decltype("']"_compStr)>;
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
    using type = typename charArrayToTypeInter<decltype(""_compStr),0,len,buf>::type;
};

template<const char* buf >
struct valueToCompStringInter<const char*,buf>{
    using type = typename charArrayToType<buf,detail::getCstrLength<buf>()>::type;
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