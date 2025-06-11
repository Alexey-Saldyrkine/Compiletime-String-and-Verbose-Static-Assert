#include "Verbose static assert.hpp"
#include <iostream>


template<typename data, size_t i,typename ret>
struct are_all_same_VSA_message_impl{
    static constexpr auto f(){
        static_assert(std::is_same_v<typename data::template typeAt<0>,int>);


        if constexpr(data::argSize <i){
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
    using typeData = typename verbose_static_assertNS::verbose_static_assert_messageData<T>;

    using msg = typename are_all_same_VSA_message_impl<typeData,1,decltype("\nFound diffreant types:\n"_compStr)>::type;
};

template<typename T,typename... U>
struct are_all_same{
    static constexpr bool value = ((std::is_same_v<T,U>)&&...);
};


template<typename typeData, size_t start,size_t i, typename ret>
struct are_all_differant_VSA_message_impl_check{
    static constexpr auto f(){
       
        if constexpr( i > typeData::argSize){
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
        if constexpr(i>=typeData::argSize){
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

template<typename... T>
struct type_set{
    template<typename U>
    static constexpr bool contains = ((std::is_same_v<U,T>)||...);
    template<typename U>
    using add = type_set<T...,U>;
};

template<typename T>
struct are_all_differant_VSA_message{
    using typeData = typename verbose_static_assertNS::verbose_static_assert_messageData<T>;
    using typeSet = type_set<>;
    using msg = typename are_all_differant_VSA_message_impl<typeData,typeSet,0,decltype("\nSame types found:\n"_compStr)>::type;
};

template<typename T,typename...U>
struct are_all_differant{
    static constexpr bool value = !((std::is_same_v<T,U>)||...) && are_all_differant<U...>::value;
};

template<typename T>
struct are_all_differant<T>{
    static constexpr bool value = 1;
};

int main() {
 
    using namespace verbose_static_assertNS;
   
    using t = are_all_same<int,int,char,float,int,double,int,long int,int>;
    verbose_static_assert<t,are_all_same_VSA_message>{};
    using y = are_all_differant<int,long int,int, char,float,double,char,double>;
    verbose_static_assert<y,are_all_differant_VSA_message>{};


    return 0;
}