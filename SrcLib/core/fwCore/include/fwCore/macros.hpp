/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/** 
 * @file fwCore/macros.hpp
 * @brief This file defines fwCore base macros.
 *
 *
 * @author IRCAD (Research and Development Team).
 */

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>


#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/comparison/equal.hpp>
#include <boost/preprocessor/comparison/greater.hpp>
#include <boost/preprocessor/control/expr_if.hpp>
#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/control/while.hpp>
#include <boost/preprocessor/facilities/empty.hpp>
#include <boost/preprocessor/facilities/expand.hpp>
#include <boost/preprocessor/facilities/identity.hpp>
#include <boost/preprocessor/logical/and.hpp>
#include <boost/preprocessor/logical/not.hpp>
#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/seq/cat.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/seq/fold_right.hpp>
#include <boost/preprocessor/seq/fold_right.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/seq/reverse.hpp>
#include <boost/preprocessor/seq/seq.hpp>
#include <boost/preprocessor/seq/to_tuple.hpp>
#include <boost/preprocessor/seq/transform.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/tuple/elem.hpp>

#include "fwCore/Demangler.hpp"

#ifdef _WIN32
#pragma warning(disable: 4003)
#endif //_WIN32



/** @cond 
 */

#define __FWCORE_TYPEDEF_SELF_NAME               SelfType
#define __FWCORE_TYPEDEF_SHARED_PTR_NAME         sptr
#define __FWCORE_TYPEDEF_WEAK_PTR_NAME           wptr
#define __FWCORE_TYPEDEF_SHARED_PTR_CONST_NAME   csptr
#define __FWCORE_TYPEDEF_WEAK_PTR_CONST_NAME     cwptr
#define __FWCORE_TYPEDEF_SHARED_PTR_FACTORY_NAME NewSptr
#define __FWCORE_FACTORY_NAME                    New
#define __FWCORE_TYPEDEF_SUPERCLASS_NAME         BaseClass
#define __FWCORE_TYPEDEF_ROOTCLASS_NAME          RootClass
#define __FWCORE_DYNAMIC_CAST_FUNC_NAME          dynamicCast
#define __FWCORE_DYNAMIC_CONST_CAST_FUNC_NAME    dynamicConstCast
#define __FWCORE_CONST_CAST_FUNC_NAME            constCast
#define __FWCORE_SPTR_FROM_THIS_FUNC_NAME        getSptr
#define __FWCORE_CONST_SPTR_FROM_THIS_FUNC_NAME  getConstSptr
#define __FWCORE_SHARED_PTR_FACTORY_CLASSNAME    __SelfNewSptr__
#define __FWCORE_ARG_NAME                        __var
/** @endcond */


/** 
 * @name Smart pointers macro
 * @{ */

/** Expand to shared_ptr < _cls_ > */
#define SPTR(_cls_)  ::boost::shared_ptr < _cls_ >
/** Expand to shared_ptr < const _cls_ > */
#define CSPTR(_cls_) ::boost::shared_ptr < const _cls_ >
/** Expand to weak_ptr < _cls_ > */
#define WPTR(_cls_)  ::boost::weak_ptr   < _cls_ >
/** Expand to weak_ptr < const _cls_ > */
#define CWPTR(_cls_) ::boost::weak_ptr   < const _cls_ >
/**  @} */



// @cond
#define __FWCORE_NOTHING( _a_ )
#define __FWCORE_NOTHING_2( _a_, _b_ )

#define __FWCORE_GET_NTH_ELEM_III(_s_, _n_, _elem_) BOOST_PP_SEQ_ELEM(_n_, _elem_)
#define __FWCORE_GET_ARGS_TYPES(_args_) BOOST_PP_SEQ_TRANSFORM(__FWCORE_GET_NTH_ELEM_III, 0, _args_)
#define __FWCORE_GET_ARG_DEFAULT_IF_HAS_ONE(arg)             \
    BOOST_PP_EXPR_IF(                                        \
            BOOST_PP_EQUAL( BOOST_PP_SEQ_SIZE(arg), 2 ),     \
            =                                                \
            )                                                \
    BOOST_PP_EXPAND(                                         \
            BOOST_PP_IF(                                     \
                BOOST_PP_EQUAL( BOOST_PP_SEQ_SIZE(arg), 2 ), \
                BOOST_PP_SEQ_ELEM, __FWCORE_NOTHING_2        \
                )                                            \
                (1,arg)                                      \
                )


#define __FWCORE_SEQ_IS_EMPTY_SINGLE(_seq_) BOOST_PP_AND( BOOST_PP_EQUAL(BOOST_PP_SEQ_SIZE(_seq_), 1), BOOST_PP_EQUAL(BOOST_PP_SEQ_SIZE(BOOST_PP_SEQ_ELEM(0,_seq_)), 0) )
#define __FWCORE_SEQ_IS_NOT_EMPTY_SINGLE(_seq_) BOOST_PP_NOT(__FWCORE_SEQ_IS_EMPTY_SINGLE(_seq_))

/*
 * __FWCORE_GENERATE_ARGS( ((a)) ((b)) ((c)(default)) ) expands to  (a __var, ) (b __var, ) (c __var, = default) where __var is defined by __FWCORE_ARG_NAME
 */
#define __FWCORE_ADD_ARG_NAME_AND_VALUE_III(s, _data_, _elem_)                                                                 \
    BOOST_PP_EXPR_IF(_data_, (BOOST_PP_SEQ_ELEM(0,_elem_) __FWCORE_ARG_NAME ,  __FWCORE_GET_ARG_DEFAULT_IF_HAS_ONE(_elem_) ) )
#define __FWCORE_GENERATE_ARGS(_args_)                                                                                                                                   \
    BOOST_PP_IF(                                                                                                                                                         \
            __FWCORE_SEQ_IS_EMPTY_SINGLE(_args_),                                                                                                                        \
            (),                                                                                                                                                          \
            BOOST_PP_EXPR_IF(__FWCORE_SEQ_IS_NOT_EMPTY_SINGLE(_args_),BOOST_PP_SEQ_TRANSFORM)  (__FWCORE_ADD_ARG_NAME_AND_VALUE_III, BOOST_PP_SEQ_SIZE(_args_), _args_ ) \
            )


/*
 * __FWCORE_GENERATE_TYPED_NUMBERED_TUPLE( ((a)) ((b)) ((c)(default)) ) expands to  (a __var0, b __var1, c __var2 = default)  or to () if _args_ is an empty boost_pp sequence
 */
#define __FWCORE_ADD_ID(_r_, _data_, _i_, _elem_) BOOST_PP_IF(_data_, ( BOOST_PP_CAT( BOOST_PP_TUPLE_ELEM(2, 0, _elem_), _i_) BOOST_PP_TUPLE_ELEM(2, 1, _elem_)), ())
#define __FWCORE_GENERATE_TYPED_NUMBERED_TUPLE(_args_)                                                                            \
    BOOST_PP_IF(                                                                                                                  \
        __FWCORE_SEQ_IS_EMPTY_SINGLE(_args_),                                                                                     \
        (),                                                                                                                       \
        BOOST_PP_SEQ_TO_TUPLE(BOOST_PP_SEQ_FOR_EACH_I(__FWCORE_ADD_ID, BOOST_PP_SEQ_SIZE(_args_),__FWCORE_GENERATE_ARGS(_args_))) \
        )


/*
 * __FWCORE_GENERATE_NUMBERED_TUPLE((a)(b)(c)) expands to  ( __var0, __var1, __var2)  or to () if _args_ is an empty boost_pp sequence
 */
#define __FWCORE_GENERATE_NUMBERED_TUPLE(_args_)                                 \
    BOOST_PP_IF(                                                                 \
            __FWCORE_SEQ_IS_EMPTY_SINGLE(_args_),                                \
            (),                                                                  \
            (BOOST_PP_ENUM_PARAMS(BOOST_PP_SEQ_SIZE(_args_), __FWCORE_ARG_NAME)) \
            )

/*
 * __FWCORE_GENERATE_CONSTRUCTOR(_factory_, _classname_, superclassname, _args_)
 *
 *  - _args_ is a boost_pp sequence of (0, 1 or 2)-elem sequences
 *
 *  expands to  _classname_ (_args_) : superclassname ( _factory_ (_args_) ) {}
 *
 *  ex: __FWCORE_GENERATE_CONSTRUCTOR( new obj, MyClass, MySuperClass, ((int)) ((float)(5.))  )
 *  expands to : MyClass(int __var0, float __var2 = 5.) : MySuperClass( new obj(__var0, __var1)) {}
 *
 *  ex: __FWCORE_GENERATE_CONSTRUCTOR( new obj, MyClass, MySuperClass, ((int)) ((string)) )
 *  expands to : MyClass(int __var0, string __var2) : MySuperClass( new obj(__var0, __var1)) {}
 *
 *  and: __FWCORE_GENERATE_CONSTRUCTOR( new obj, MyClass, MySuperClass, ())
 *  expands to : MyClass() : MySuperClass( new obj()) {}
 *
 */
#define __FWCORE_GENERATE_CONSTRUCTOR( _factory_, _classname_, superclassname, _args_ )                                                   \
    _classname_ __FWCORE_GENERATE_TYPED_NUMBERED_TUPLE(_args_) : superclassname ( _factory_ __FWCORE_GENERATE_NUMBERED_TUPLE(_args_) ) {}


/*
 * __FWCORE_GENERATE_ONE_CONSTRUCTOR_III(_r_, _data_, _args_)
 *
 * wrapper of __FWCORE_GENERATE_CONSTRUCTOR for __FWCORE_GENERATE_CONSTRUCTORS_IV needs
 * _args_ : ( boost-needed, (factory) (classname) (superclassname), _args_)
 */
#define __FWCORE_GENERATE_ONE_CONSTRUCTOR_III(_r_, _data_, _args_)                                                                 \
     __FWCORE_GENERATE_CONSTRUCTOR( BOOST_PP_SEQ_ELEM(0,_data_),BOOST_PP_SEQ_ELEM(1,_data_),BOOST_PP_SEQ_ELEM(2,_data_), _args_) ;

/*
 * __FWCORE_GENERATE_CONSTRUCTORS_IV( _factory_, _classname_, superclassname, _args_ )
 *
 * generates a constructor for each item of _args_ using "_factory_"
 *
 */
#define __FWCORE_GENERATE_CONSTRUCTORS_IV( _factory_, _classname_, superclassname, _args_ )                          \
    BOOST_PP_SEQ_FOR_EACH(__FWCORE_GENERATE_ONE_CONSTRUCTOR_III, (_factory_) (_classname_) (superclassname), _args_)


/*
 * __FWCORE_GENERATE_ONE_CONSTRUCTOR_WITH_N_FACTORIES_III(_r_, _data_, _factory_args_)
 *
 * wrapper of __FWCORE_GENERATE_CONSTRUCTOR for __FWCORE_GENERATE_CONSTRUCTORS_WITH_N_FACTORIES_III usage
 *
 * _factory_args_ format : (factory, ((type1)) ((type2)) ... ((typeN)) )
 */
#define __FWCORE_GENERATE_ONE_CONSTRUCTOR_WITH_N_FACTORIES_III(_r_, _data_, _factory_args_) \
     __FWCORE_GENERATE_CONSTRUCTOR(                                                         \
             BOOST_PP_TUPLE_ELEM(2,0,_factory_args_),                                       \
             BOOST_PP_SEQ_ELEM(0,_data_),                                                   \
             BOOST_PP_SEQ_ELEM(1,_data_),                                                   \
             BOOST_PP_TUPLE_ELEM(2,1,_factory_args_)                                        \
             );

/*
 * __FWCORE_GENERATE_CONSTRUCTORS_WITH_N_FACTORIES_III( _classname_, superclassname, _factories_args_ )
 *
 * generates a constructor for each item of _args_ using a different specified factory
 *
 * ex. of _factories_args_ : ((new obj, () )) ((new objFromInt, ((int)) ))
 *
 */
#define __FWCORE_GENERATE_CONSTRUCTORS_WITH_N_FACTORIES_III( _classname_, superclassname, _factories_args_ )                        \
    BOOST_PP_SEQ_FOR_EACH(__FWCORE_GENERATE_ONE_CONSTRUCTOR_WITH_N_FACTORIES_III, (_classname_) (superclassname), _factories_args_)


/*
 * __FWCORE_GENERATE_CONSTRUCTORS( _classname_ , _factory_, _args_ )
 *
 * Wrapper for __FWCORE_GENERATE_CONSTRUCTORS_IV
 *
 */
#define __FWCORE_GENERATE_CONSTRUCTORS( _classname_ , _factory_, _args_) \
    __FWCORE_GENERATE_CONSTRUCTORS_IV(                                   \
            _factory_,                                                   \
            __FWCORE_SHARED_PTR_FACTORY_CLASSNAME,                       \
            __FWCORE_TYPEDEF_SHARED_PTR_NAME,                            \
            _args_                                                       \
            )

/*
 * __FWCORE_GENERATE_CONSTRUCTORS_WITH_N_FACTORIES( _classname_ , _factories_args_ )
 *
 * Wrapper for __FWCORE_GENERATE_CONSTRUCTORS_WITH_N_FACTORIES_III *
 */
#define __FWCORE_GENERATE_CONSTRUCTORS_WITH_N_FACTORIES( _classname_ , _factories_args_ ) \
    __FWCORE_GENERATE_CONSTRUCTORS_WITH_N_FACTORIES_III(                                  \
            __FWCORE_SHARED_PTR_FACTORY_CLASSNAME,                                        \
            __FWCORE_TYPEDEF_SHARED_PTR_NAME,                                             \
            _factories_args_                                                              \
            )



/*
 * Utils for Factories (obj::New(...)), work like constructors utils
 */
#define __FWCORE_GENERATE_FACTORY( _factory_, _args_ )                                                           \
    static __FWCORE_TYPEDEF_SHARED_PTR_NAME __FWCORE_FACTORY_NAME __FWCORE_GENERATE_TYPED_NUMBERED_TUPLE(_args_) \
        {                                                                                                        \
            return __FWCORE_TYPEDEF_SHARED_PTR_NAME (_factory_ __FWCORE_GENERATE_NUMBERED_TUPLE(_args_)) ;       \
        }


#define __FWCORE_GENERATE_ONE_FACTORY(_r_, _data_, _args_)             \
     __FWCORE_GENERATE_FACTORY( BOOST_PP_SEQ_ELEM(0,_data_), _args_) ;


#define __FWCORE_GENERATE_FACTORIES( _factory_, _args_ )                      \
    BOOST_PP_SEQ_FOR_EACH(__FWCORE_GENERATE_ONE_FACTORY, (_factory_), _args_)


#define __FWCORE_GENERATE_ONE_FACTORY_WITH_N_FACTORIES(_r_, _data_, _factory_args_)                                \
    __FWCORE_GENERATE_FACTORY( BOOST_PP_TUPLE_ELEM(2,0,_factory_args_), BOOST_PP_TUPLE_ELEM(2,1,_factory_args_)) ;


#define __FWCORE_GENERATE_FACTORIES_WITH_N_FACTORIES_I( _factories_args_ )                                    \
    BOOST_PP_SEQ_FOR_EACH(__FWCORE_GENERATE_ONE_FACTORY_WITH_N_FACTORIES, (BOOST_PP_EMPTY), _factories_args_)


#define __FWCORE_GENERATE_FACTORIES_WITH_ONE_FACTORY( _factory_, _args_ ) \
    __FWCORE_GENERATE_FACTORIES( _factory_, _args_ )


#define __FWCORE_GENERATE_FACTORIES_WITH_N_FACTORIES( _factories_args_ ) \
    __FWCORE_GENERATE_FACTORIES_WITH_N_FACTORIES_I( _factories_args_ )


/*
 * __FWCORE_GET_CLASSNAME and __FWCORE_GET_SUPERCLASSNAME work with a sequence like "(classname)(baseclassname)" or "(classname)"
 * and return respectively 1st and 2nd element
 *
 * if the sequence containt only 1 element, __FWCORE_GET_SUPERCLASSNAME returns the 1st
 */
#define __FWCORE_GET_CLASSNAME( _seq_ ) BOOST_PP_SEQ_ELEM(0, _seq_)
#define __FWCORE_GET_SUPERCLASSNAME( _seq_ ) BOOST_PP_SEQ_ELEM( BOOST_PP_IF( BOOST_PP_EQUAL( BOOST_PP_SEQ_SIZE(_seq_), 2 ), 1, 0), _seq_)


/*
 * __FWCORE_CLASS_TYPEDEFS define several typdefs for classes (sptr, wptr, ...)
 *
 * Baseclass is a typedef to the superclass
 * Rootclass is a typedef to the toplevel base class
 */
#define __FWCORE_CLASS_TYPEDEFS(_classinfo_)                                                               \
    /** Self type  */                                                                                      \
    typedef __FWCORE_GET_CLASSNAME(_classinfo_) __FWCORE_TYPEDEF_SELF_NAME;                                \
    /** Type of base class  */                                                                             \
    typedef  BOOST_PP_IF( BOOST_PP_EQUAL( BOOST_PP_SEQ_SIZE(_classinfo_), 2 ),                             \
                                        __FWCORE_GET_SUPERCLASSNAME(_classinfo_),                          \
                                        __FWCORE_TYPEDEF_SELF_NAME                                         \
                                        ) __FWCORE_TYPEDEF_SUPERCLASS_NAME ;                               \
                                                                                                           \
    typedef  BOOST_PP_IF( BOOST_PP_EQUAL( BOOST_PP_SEQ_SIZE(_classinfo_), 2 ),                             \
                                        __FWCORE_TYPEDEF_SUPERCLASS_NAME::__FWCORE_TYPEDEF_ROOTCLASS_NAME, \
                                        __FWCORE_TYPEDEF_SELF_NAME                                         \
                                        ) __FWCORE_TYPEDEF_ROOTCLASS_NAME ;                                \
    /** Shared pointer type  */                                                                            \
    typedef SPTR ( __FWCORE_TYPEDEF_SELF_NAME ) __FWCORE_TYPEDEF_SHARED_PTR_NAME;                          \
    /** Weak pointer type  */                                                                              \
    typedef WPTR ( __FWCORE_TYPEDEF_SELF_NAME ) __FWCORE_TYPEDEF_WEAK_PTR_NAME;                            \
    /** Const shared pointer type  */                                                                      \
    typedef CSPTR( __FWCORE_TYPEDEF_SELF_NAME ) __FWCORE_TYPEDEF_SHARED_PTR_CONST_NAME;                    \
    /** Const weak pointer type  */                                                                        \
    typedef CWPTR( __FWCORE_TYPEDEF_SELF_NAME ) __FWCORE_TYPEDEF_WEAK_PTR_CONST_NAME

/*
 * Cast definition for casting from baseclassname and derived to _classname_
 */
#define __FWCORE_GENERATE_CAST(_classname_, _baseclassname_)                                                                             \
    /** @brief Cast to dynamic shared pointer   */                                                                                       \
    template< class BASETYPE > static __FWCORE_TYPEDEF_SHARED_PTR_NAME __FWCORE_DYNAMIC_CAST_FUNC_NAME ( BASETYPE const &p )             \
    {                                                                                                                                    \
        return ::boost::dynamic_pointer_cast< _classname_ >(p);                                                                          \
    };                                                                                                                                   \
    /** @brief Const shared pointer cast to dynamic pointer */                                                                           \
    template< class BASETYPE > static __FWCORE_TYPEDEF_SHARED_PTR_CONST_NAME __FWCORE_DYNAMIC_CONST_CAST_FUNC_NAME ( BASETYPE const &p ) \
    {                                                                                                                                    \
        return ::boost::dynamic_pointer_cast< const _classname_ >(p);                                                                    \
    };                                                                                                                                   \
    /** @brief Cast to const shared pointer */                                                                                           \
    static __FWCORE_TYPEDEF_SHARED_PTR_NAME __FWCORE_CONST_CAST_FUNC_NAME ( __FWCORE_TYPEDEF_SHARED_PTR_CONST_NAME const &p )            \
    {                                                                                                                                    \
        return ::boost::const_pointer_cast< _classname_ >(p);                                                                            \
    }


#define __FWCORE_PREDECLARE(_s_, _state_, _elem_) \
     BOOST_PP_IF(                                 \
                BOOST_PP_EQUAL( _s_, 2 ),         \
                class _elem_; ,                   \
                namespace _elem_ { _state_ }      \
               )

// @endcond

/** 
 * @name Class declaration helpers
 * @{ */

/**
 * @brief Generate getSptr and getConstSptr methods
 *
 * These methods use 'shared_from_this' to get a shared pointer and cast it to
 * required type
 *
 */
#define fwCoreAllowSharedFromThis()                                                                   \
    /** @brief return a casted const shared ptr from this object */                                   \
    __FWCORE_TYPEDEF_SHARED_PTR_CONST_NAME __FWCORE_CONST_SPTR_FROM_THIS_FUNC_NAME() const {          \
        return __FWCORE_DYNAMIC_CONST_CAST_FUNC_NAME(this->::fwCore::BaseObject::shared_from_this()); \
    }                                                                                                 \
    /** @brief return a casted shared ptr from this object */                                         \
    __FWCORE_TYPEDEF_SHARED_PTR_NAME __FWCORE_SPTR_FROM_THIS_FUNC_NAME(){                             \
        return __FWCORE_DYNAMIC_CAST_FUNC_NAME(this->::fwCore::BaseObject::shared_from_this());       \
    }

/**
 * @brief Generate predeclarations
 *
 * @param _cls_ Class to predeclare, in the form (some)(namespace)(class)
 *
 *   Example :
 *   fwCorePredeclare( (data)(Image) ) expands to :
 *   namespace fwData {
 *     class Image;
 *     }
 *
 */
#define fwCorePredeclare( _cls_ )                                           \
    BOOST_PP_SEQ_FOLD_RIGHT( __FWCORE_PREDECLARE, BOOST_PP_SEQ_NIL, _cls_)


#define __FWCORE_STATIC_CACHE( _type_, value )        \
    static _type_ __cache__(value); return __cache__;

/**
 * @brief Generate virtual methods that return classname/namespace strings
 *
 * Example: for ::fwData::Object,
 * - Classname is Object
 * - FullClassname is ::fwData::Object
 * - RootedClassname is ::fwData::Object
 * - FullNamespace is ::fwData
 * - RootedNamespace is ::fwData
 *
 */
#define fwCoreClassnameMacro()                                                                                                                          \
    /** @name Demangling methods */                                                                                                                     \
    /** @{ */                                                                                                                                           \
    /** @brief return object's classname given by ::fwCore::Demangler::getLeafClassname()  */                                                           \
    virtual const std::string getLeafClassname()   const    { __FWCORE_STATIC_CACHE(std::string,  ::fwCore::Demangler(*this).getLeafClassname());};     \
    static  const std::string leafClassname()            { __FWCORE_STATIC_CACHE(std::string,  ::fwCore::getLeafClassname<SelfType>());};               \
    /** @brief return object's classname given by ::fwCore::Demangler::getClassname()  */                                                               \
    virtual const std::string getClassname()       const    { __FWCORE_STATIC_CACHE(std::string,  ::fwCore::Demangler(*this).getClassname());};         \
    static  const std::string classname()                { __FWCORE_STATIC_CACHE(std::string,  ::fwCore::getClassname<SelfType>());};                   \
    /** @brief return object's classname given by ::fwCore::Demangler::getFullClassname()  */                                                           \
    virtual const std::string getFullClassname()   const    { __FWCORE_STATIC_CACHE(std::string,  ::fwCore::Demangler(*this).getFullClassname());};     \
    static  const std::string fullClassname()            { __FWCORE_STATIC_CACHE(std::string,  ::fwCore::getFullClassname<SelfType>());};               \
    /** @brief return object's classname given by ::fwCore::Demangler::getRootedClassname()  */                                                         \
    virtual const std::string getRootedClassname() const    { __FWCORE_STATIC_CACHE(std::string,  ::fwCore::Demangler(*this).getRootedClassname());};   \
    static  const std::string rootedClassname()          { __FWCORE_STATIC_CACHE(std::string,  ::fwCore::getRootedClassname<SelfType>());};             \
                                                                                                                                                        \
    /** @brief return object's namespace given by ::fwCore::Demangler::getFullClassname()  */                                                           \
    virtual const std::string getFullNamespace()   const    { __FWCORE_STATIC_CACHE(std::string,  ::fwCore::Demangler(*this).getFullClassname());};     \
    static  const std::string fullNamespace()            { __FWCORE_STATIC_CACHE(std::string,  ::fwCore::getFullClassname<SelfType>());};               \
    /** @brief return object's namespace given by ::fwCore::Demangler::getRootedNamespace()  */                                                         \
    virtual const std::string getRootedNamespace() const    { __FWCORE_STATIC_CACHE(std::string,  ::fwCore::Demangler(*this).getRootedClassname());}    \
    static  const std::string rootedNamespace()          { __FWCORE_STATIC_CACHE(std::string,  ::fwCore::getRootedClassname<SelfType>());}              \
     /** @} */


/**
 * @brief Generate common construction methods for classes with one factory
 *
 * For this macro parameters, each bracket is significant.
 * @param _classinfo_ Class information in the form : (classname)(baseclassname) or (classname).
 *                  baseclassname is only required for a non-base class, and should not
 *                  be used if baseclassname == classname
 *
 * @param _parameters_ ( types_param_factory0 ) (...) ( types_param_factoryN )
 *                      - where types_param_factoryX can be :
 *                        - for a 0-arg factory : ()
 *                        - for a 1-arg of T type factory : ((T)) or ((T)(d))
 *                          where d is a defautl value
 *                        - for a N-parameters factory : ((type0)) ((type1)) ... ((typeN)(default_value))
 *                      - Several types_param_factory can be declared if a factory have several signatures
 *                        - Example for several signatures : () ( ((int)) ) ( ((int)) ((std::string)("default")) )
 *
 * @param _factory_ A factory that can take arguments as defined in _parameters_ arguments
 */
#define fwCoreClassDefinitionsWithFactoryMacro(_classinfo_, _parameters_, _factory_)                                                                                     \
    __FWCORE_CLASS_TYPEDEFS(_classinfo_);                                                                                                                                \
    /* @cond */                                                                                                                                                          \
    class __FWCORE_SHARED_PTR_FACTORY_CLASSNAME: public __FWCORE_TYPEDEF_SHARED_PTR_NAME                                                                                 \
    {                                                                                                                                                                    \
        public:                                                                                                                                                          \
          __FWCORE_GENERATE_CONSTRUCTORS (__FWCORE_GET_CLASSNAME(_classinfo_), _factory_, _parameters_);                                                                 \
    };                                                                                                                                                                   \
    /* @endcond */                                                                                                                                                       \
    /** Specialized version of shared_ptr (alias to shared_ptr< __FWCORE_GET_CLASSNAME(_classinfo_) > ) with embeded factory for __FWCORE_GET_CLASSNAME(_classinfo_). */ \
    typedef  __FWCORE_SHARED_PTR_FACTORY_CLASSNAME __FWCORE_TYPEDEF_SHARED_PTR_FACTORY_NAME;                                                                             \
    /* @cond */                                                                                                                                                          \
    /* @endcond */                                                                                                                                                       \
    __FWCORE_GENERATE_FACTORIES_WITH_ONE_FACTORY (_factory_, _parameters_);                                                                                              \
    __FWCORE_GENERATE_CAST(__FWCORE_GET_CLASSNAME(_classinfo_), __FWCORE_TYPEDEF_ROOTCLASS_NAME);                                                                        \
    fwCoreClassnameMacro()


/**
 * @brief Generate common construction methods for classes with several factories
 *
 * For this macro parameters, each bracket is significant.
 * @param _classinfo_ Class information in the form : (classname)(baseclassname) or (classname).
 *                  baseclassname is only required for a non-base class, and should not
 *                  be used if baseclassname == classname
 *
 * @param _factories_args_ ((factory0, types_param_factory0)) ((factory1, types_param_factory1) ... ((factoryN, types_param_factoryN))
 *                          - where types_param_factoryX can be :
 *                            - for a 0-arg factory : ()
 *                            - for a 1-arg of T type factory : ((T)) or ((T)(d))
 *                              where d is a defautl value
 *                            - for a N-parameters factory : ((type0)) ((type1)) ... ((typeN)(default_value))
 *
 */
#define fwCoreClassDefinitionsWithNFactoriesMacro(_classinfo_, _factories_args_)                                                                                         \
    __FWCORE_CLASS_TYPEDEFS(_classinfo_);                                                                                                                                \
    /* @cond */                                                                                                                                                          \
    class __FWCORE_SHARED_PTR_FACTORY_CLASSNAME: public __FWCORE_TYPEDEF_SHARED_PTR_NAME                                                                                 \
    {                                                                                                                                                                    \
        public:                                                                                                                                                          \
          __FWCORE_GENERATE_CONSTRUCTORS_WITH_N_FACTORIES (__FWCORE_GET_CLASSNAME(_classinfo_), _factories_args_);                                                       \
    } ;                                                                                                                                                                  \
    /* @endcond */                                                                                                                                                       \
    /** Specialized version of shared_ptr (alias to shared_ptr< __FWCORE_GET_CLASSNAME(_classinfo_) > ) with embeded factory for __FWCORE_GET_CLASSNAME(_classinfo_). */ \
    typedef __FWCORE_SHARED_PTR_FACTORY_CLASSNAME __FWCORE_TYPEDEF_SHARED_PTR_FACTORY_NAME;                                                                              \
    /* @cond */                                                                                                                                                          \
                                                                                                                                                                         \
    __FWCORE_GENERATE_FACTORIES_WITH_N_FACTORIES (_factories_args_);                                                                                                     \
    /* @endcond */                                                                                                                                                       \
    __FWCORE_GENERATE_CAST(__FWCORE_GET_CLASSNAME(_classinfo_),__FWCORE_TYPEDEF_ROOTCLASS_NAME);                                                                         \
    fwCoreClassnameMacro()


/**
 * @brief Generate common code for services classes
 *
 * @param _classinfo_ Class information in the form : (classname)(baseclassname) or (classname).
 *                  baseclassname is only required for a non-base class, and should not
 *                  be used if baseclassname == classname
 */
#define fwCoreServiceClassDefinitionsMacro(_classinfo_)                                           \
    __FWCORE_CLASS_TYPEDEFS(_classinfo_);                                                         \
    __FWCORE_GENERATE_CAST(__FWCORE_GET_CLASSNAME(_classinfo_), __FWCORE_TYPEDEF_ROOTCLASS_NAME); \
    fwCoreClassnameMacro()




/**
 * @brief Generate common code for Non Instanciable classes (Interfaces, Abstrat classes, ...)
 *

 * @param _classinfo_ Class information in the form : (classname)(baseclassname) or (classname).
 *                  baseclassname is only required for a non-base class, and should not
 *                  be used if baseclassname == classname
 */
#define fwCoreNonInstanciableClassDefinitionsMacro(_classinfo_)                                   \
    __FWCORE_CLASS_TYPEDEFS(_classinfo_);                                                         \
    __FWCORE_GENERATE_CAST(__FWCORE_GET_CLASSNAME(_classinfo_), __FWCORE_TYPEDEF_ROOTCLASS_NAME); \
    fwCoreClassnameMacro()



/**  @} */

