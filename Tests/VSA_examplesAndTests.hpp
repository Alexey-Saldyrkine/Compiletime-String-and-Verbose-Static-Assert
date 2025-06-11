#include "verbose_static_assert.hpp"

template<typename data, size_t i,typename ret>
struct are_all_same_VSA_message_impl{
    static constexpr auto f(){
        static_assert(std::is_same_v<typename data::template typeAt<0>,int>);


        if constexpr(data::size <=i){
            return ret{};
        }else{
            if constexpr(!std::is_same_v<typename data::template typeAt<0>,typename data::template typeAt<i>>){
            using namespace compStringNS::compStringConvNS;
            using msg = typename decltype("type "_compStr)::append<typename data::template typeStringAt<i>>
            ::template append<decltype(" != "_compStr)>::template append< typename data::template typeStringAt<0> >
            ::template append<decltype(" at pos "_compStr)>::template append<typename valueToCompString<i>::type >::template append<decltype(", \n"_compStr)>;
            using retT = typename ret::template append<msg>;
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
            using appMsg = typename decltype("found same type "_compStr)::append<typeName>
            ::template append<decltype(" at pos "_compStr)>:: template append<typename valueToCompString<start>::type>
            ::template append<decltype(" and "_compStr)>:: template append<typename valueToCompString<i>::type>::template append<decltype(", \n"_compStr)>;
            using retT = typename ret::template append<appMsg>;
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
            using appMsg = typename decltype("At pos "_compStr)
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
            using rmsg = typename msg::template append<appMsg>;
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
            if constexpr(!test_comparitor_ab<comparitor,typename T::template typeAt<indx>, typename T::template typeAt<offset>>::value){
                using addOnMessage = typename decltype("could not create the comparitor type '"_compStr)
                ::template append<typename typeToCompString<comparitor<int,int>>::template templateName<>>
                ::template append<decltype("' with the types '"_compStr)>
                ::template append<typename typeToCompString<typename T::template typeAt<indx>>::type>
                ::template append< decltype("' and '"_compStr)>
                ::template append< typename typeToCompString<typename T::template typeAt<offset>>::type >
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
            using newMsg =  typename is_comparitor_for_template_pack_VSA_message_line_impl<T,typename T::template typeAt<0>, indx,indx+1,msg>::type;
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

namespace compStringNS::compStringConvNS{

        namespace typeToStringDefinitions{
            createTypeDefinition(some_none_fundamental_type);
        }

        namespace templatedTypeDefinitions{
            template< typename A, typename B>
            struct templatedTypeToCompString<myComparitor<A,B>>{
            using typeList = mp_list<A,B>;
            using typeName = decltype("myComparitor"_compStr);
            };
        }     

}

template<typename U>
int showVSAExamples() {
 
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
