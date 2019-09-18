/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

/**
 * @brief This file defines fwCore legacy base macros.
 */

#pragma once

#include <boost/preprocessor/comparison/equal.hpp>
#include <boost/preprocessor/control/expr_if.hpp>
#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/facilities/empty.hpp>
#include <boost/preprocessor/facilities/expand.hpp>
#include <boost/preprocessor/logical/and.hpp>
#include <boost/preprocessor/logical/not.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/seq/elem.hpp>
#include <boost/preprocessor/seq/fold_right.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/seq/seq.hpp>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/seq/to_tuple.hpp>
#include <boost/preprocessor/seq/transform.hpp>
#include <boost/preprocessor/tuple/elem.hpp>

#ifdef _WIN32
#pragma warning(disable: 4003)
#endif //_WIN32

/** @cond
 */

#define __FWCORE_TYPEDEF_SELF_NAME               SelfType
#define __FWCORE_TYPEDEF_SHARED_PTR_NAME         sptr
#define __FWCORE_TYPEDEF_WEAK_PTR_NAME           wptr
#define __FWCORE_TYPEDEF_UNIQUE_PTR_NAME         uptr
#define __FWCORE_TYPEDEF_SHARED_PTR_CONST_NAME   csptr
#define __FWCORE_TYPEDEF_WEAK_PTR_CONST_NAME     cwptr
#define __FWCORE_TYPEDEF_UNIQUE_PTR_CONST_NAME   cuptr
#define __FWCORE_FACTORY_NAME                    New
#define __FWCORE_TYPEDEF_SUPERCLASS_NAME         BaseClass
#define __FWCORE_TYPEDEF_ROOTCLASS_NAME          RootClass
#define __FWCORE_DYNAMIC_CAST_FUNC_NAME          dynamicCast
#define __FWCORE_DYNAMIC_CONST_CAST_FUNC_NAME    dynamicConstCast
#define __FWCORE_CONST_CAST_FUNC_NAME            constCast
#define __FWCORE_SPTR_FROM_THIS_FUNC_NAME        getSptr
#define __FWCORE_CONST_SPTR_FROM_THIS_FUNC_NAME  getConstSptr
#define __FWCORE_ARG_NAME                        __var
/** @endcond */

// @cond
#define __FWCORE_NOTHING( _a_ )
#define __FWCORE_NOTHING_2( _a_, _b_ )

#define __FWCORE_GET_NTH_ELEM_III(_s_, _n_, _elem_) BOOST_PP_SEQ_ELEM(_n_, _elem_)
#define __FWCORE_GET_ARGS_TYPES(_args_) BOOST_PP_SEQ_TRANSFORM(__FWCORE_GET_NTH_ELEM_III, 0, _args_)
#define __FWCORE_GET_ARG_DEFAULT_IF_HAS_ONE(arg)             \
    BOOST_PP_EXPR_IF(                                        \
        BOOST_PP_EQUAL( BOOST_PP_SEQ_SIZE(arg), 2 ),         \
            =                                                \
            )                                                \
    BOOST_PP_EXPAND(                                         \
        BOOST_PP_IF(                                         \
            BOOST_PP_EQUAL( BOOST_PP_SEQ_SIZE(arg), 2 ),     \
            BOOST_PP_SEQ_ELEM, __FWCORE_NOTHING_2            \
            )                                                \
        (1, arg)                                             \
        )

#define __FWCORE_SEQ_IS_EMPTY_SINGLE(_seq_) BOOST_PP_AND(              \
        BOOST_PP_EQUAL(BOOST_PP_SEQ_SIZE(_seq_), 1),                   \
        BOOST_PP_EQUAL(BOOST_PP_SEQ_SIZE(BOOST_PP_SEQ_ELEM(0, _seq_)), \
                       0) )
#define __FWCORE_SEQ_IS_NOT_EMPTY_SINGLE(_seq_) BOOST_PP_NOT(__FWCORE_SEQ_IS_EMPTY_SINGLE(_seq_))

/*
 * __FWCORE_GENERATE_ARGS( ((a)) ((b)) ((c)(default)) ) expands to  (a __var, ) (b __var, ) (c __var, = default) where
 *__var is defined by __FWCORE_ARG_NAME
 */
#define __FWCORE_ADD_ARG_NAME_AND_VALUE_III(s, _data_, _elem_)                                                         \
    BOOST_PP_EXPR_IF(_data_,                                                                                           \
                     (BOOST_PP_SEQ_ELEM(0, _elem_) __FWCORE_ARG_NAME,  __FWCORE_GET_ARG_DEFAULT_IF_HAS_ONE(_elem_) ) )

#define __FWCORE_GENERATE_ARGS(_args_)                                                                             \
    BOOST_PP_IF(                                                                                                   \
        __FWCORE_SEQ_IS_EMPTY_SINGLE(_args_),                                                                      \
        (),                                                                                                        \
        BOOST_PP_EXPR_IF(__FWCORE_SEQ_IS_NOT_EMPTY_SINGLE(_args_),                                                 \
                         BOOST_PP_SEQ_TRANSFORM)  (__FWCORE_ADD_ARG_NAME_AND_VALUE_III, BOOST_PP_SEQ_SIZE(_args_), \
                                                   _args_ )                                                        \
        )

/*
 * __FWCORE_GENERATE_TYPED_NUMBERED_TUPLE( ((a)) ((b)) ((c)(default)) ) expands to  (a __var0, b __var1, c __var2 =
 * default)  or to () if _args_ is an empty boost_pp sequence
 */
#define __FWCORE_ADD_ID(_r_, _data_, _i_, _elem_) BOOST_PP_IF(_data_,                                                \
                                                              ( BOOST_PP_CAT( BOOST_PP_TUPLE_ELEM(2, 0, _elem_),     \
                                                                              _i_) BOOST_PP_TUPLE_ELEM(2, 1,         \
                                                                                                       _elem_)), ())
#define __FWCORE_GENERATE_TYPED_NUMBERED_TUPLE(_args_)                                            \
    BOOST_PP_IF(                                                                                  \
        __FWCORE_SEQ_IS_EMPTY_SINGLE(_args_),                                                     \
        (),                                                                                       \
        BOOST_PP_SEQ_TO_TUPLE(BOOST_PP_SEQ_FOR_EACH_I(__FWCORE_ADD_ID, BOOST_PP_SEQ_SIZE(_args_), \
                                                      __FWCORE_GENERATE_ARGS(_args_)))            \
        )

/*
 * __FWCORE_GENERATE_NUMBERED_TUPLE((a)(b)(c)) expands to  ( __var0, __var1, __var2)  or to () if _args_ is an empty
 * boost_pp sequence
 */
#define __FWCORE_GENERATE_NUMBERED_TUPLE(_args_)                             \
    BOOST_PP_IF(                                                             \
        __FWCORE_SEQ_IS_EMPTY_SINGLE(_args_),                                \
        (),                                                                  \
        (BOOST_PP_ENUM_PARAMS(BOOST_PP_SEQ_SIZE(_args_), __FWCORE_ARG_NAME)) \
        )

/*
 * Utils for Factories (obj::New(...)), work like constructors utils
 */
#define __FWCORE_GENERATE_FACTORY( _factory_, _args_ )                                                           \
    static __FWCORE_TYPEDEF_SHARED_PTR_NAME __FWCORE_FACTORY_NAME __FWCORE_GENERATE_TYPED_NUMBERED_TUPLE(_args_) \
    {                                                                                                            \
        return __FWCORE_TYPEDEF_SHARED_PTR_NAME(_factory_ __FWCORE_GENERATE_NUMBERED_TUPLE(_args_));             \
    }

#define __FWCORE_GENERATE_ONE_FACTORY(_r_, _data_, _args_) \
    __FWCORE_GENERATE_FACTORY( BOOST_PP_SEQ_ELEM(0, _data_), _args_)

#define __FWCORE_GENERATE_FACTORIES_WITH_ONE_FACTORY( _factory_, _args_ ) \
    BOOST_PP_SEQ_FOR_EACH(__FWCORE_GENERATE_ONE_FACTORY, (_factory_), _args_)

/*
 * __FWCORE_GET_CLASSNAME and __FWCORE_GET_SUPERCLASSNAME work with a sequence like "(classname)(baseclassname)" or
 *"(classname)"
 * and return respectively 1st and 2nd element
 *
 * if the sequence containt only 1 element, __FWCORE_GET_SUPERCLASSNAME returns the 1st
 */
#define __FWCORE_GET_CLASSNAME( _seq_ ) BOOST_PP_SEQ_ELEM(0, _seq_)
#define __FWCORE_GET_SUPERCLASSNAME( _seq_ ) BOOST_PP_SEQ_ELEM( BOOST_PP_IF( BOOST_PP_EQUAL( BOOST_PP_SEQ_SIZE(_seq_), \
                                                                                             2 ), 1, 0), _seq_)

/*
 * __FWCORE_CLASS_TYPEDEFS define several typdefs for classes (sptr, wptr, ...)
 *
 * BaseClass is a typedef to the superclass
 * RootClass is a typedef to the toplevel base class
 */
#define __FWCORE_CLASS_TYPEDEFS(_classinfo_)                                                    \
    /** Self type  */                                                                           \
    typedef __FWCORE_GET_CLASSNAME (_classinfo_) __FWCORE_TYPEDEF_SELF_NAME;                    \
    /** Type of base class  */                                                                  \
    typedef  __FWCORE_GET_SUPERCLASSNAME (_classinfo_) __FWCORE_TYPEDEF_SUPERCLASS_NAME;        \
                                                                                                \
    typedef  BOOST_PP_IF ( BOOST_PP_EQUAL ( BOOST_PP_SEQ_SIZE(_classinfo_), 2 ),                \
                           __FWCORE_TYPEDEF_SUPERCLASS_NAME::__FWCORE_TYPEDEF_ROOTCLASS_NAME,   \
                           __FWCORE_TYPEDEF_SELF_NAME                                           \
                           ) __FWCORE_TYPEDEF_ROOTCLASS_NAME;                                   \
    /** Shared pointer type  */                                                                 \
    typedef SPTR ( __FWCORE_TYPEDEF_SELF_NAME ) __FWCORE_TYPEDEF_SHARED_PTR_NAME;               \
    /** Weak pointer type  */                                                                   \
    typedef WPTR ( __FWCORE_TYPEDEF_SELF_NAME ) __FWCORE_TYPEDEF_WEAK_PTR_NAME;                 \
    /** Unique pointer type  */                                                                 \
    typedef UPTR ( __FWCORE_TYPEDEF_SELF_NAME ) __FWCORE_TYPEDEF_UNIQUE_PTR_NAME;               \
    /** Const shared pointer type  */                                                           \
    typedef CSPTR ( __FWCORE_TYPEDEF_SELF_NAME ) __FWCORE_TYPEDEF_SHARED_PTR_CONST_NAME;        \
    /** Const weak pointer type  */                                                             \
    typedef CWPTR ( __FWCORE_TYPEDEF_SELF_NAME ) __FWCORE_TYPEDEF_WEAK_PTR_CONST_NAME;          \
    /** Const unique pointer type  */                                                           \
    typedef CUPTR ( __FWCORE_TYPEDEF_SELF_NAME ) __FWCORE_TYPEDEF_UNIQUE_PTR_CONST_NAME;

#define __FWCORE_PREDECLARE(_s_, _state_, _elem_) \
    BOOST_PP_IF(                                  \
        BOOST_PP_EQUAL( _s_, 2 ),                 \
        class _elem_; ,                           \
        namespace _elem_ { _state_ }              \
        )

// @endcond

/**
 * @name Class declaration helpers
 * @{ */

/**
 * @brief Generate predeclarations
 *
 * @param \_cls_ Class to predeclare, in the form (some)(namespace)(class)
 *
 *   Example :
 *   fwCorePredeclare( (fwData)(Image) ) expands to :
 *   namespace fwData {
 *     class Image;
 *     }
 *
 */
#define fwCorePredeclare( _cls_ ) \
    BOOST_PP_SEQ_FOLD_RIGHT( __FWCORE_PREDECLARE, BOOST_PP_SEQ_NIL, _cls_)

#ifdef __GNUC__
#define FW_NOINLINE __attribute__((noinline))
#else
#define FW_NOINLINE
#endif

/**
 * @brief Generate virtual methods that check if passed type is same type of
 * (or a topclass of) 'this' type
 *
 * Example:
 * ::fwData::Image::IsTypeOf("::fwData::Object") is true
 * image->isA("::fwData::Object") is true
 *
 */
#define __FWCOREISTYPEOFMACRO(_classinfo_)                                                  \
    static bool isTypeOf(const std::string& type)                                           \
    {                                                                                       \
        if (__FWCORE_TYPEDEF_SELF_NAME::classname() == type)                                \
        {                                                                                   \
            return true;                                                                    \
        }                                                                                   \
        return BOOST_PP_IF( BOOST_PP_EQUAL( BOOST_PP_SEQ_SIZE( _classinfo_), 2 ),           \
                            __FWCORE_TYPEDEF_SUPERCLASS_NAME::isTypeOf( type ), false);     \
    }

#define fwCoreIsTypeOfMacro(_classinfo_)                              \
    __FWCOREISTYPEOFMACRO(_classinfo_)                                \
    virtual bool isA(const std::string& type) const override          \
    {                                                                 \
        return __FWCORE_TYPEDEF_SELF_NAME::isTypeOf(type);            \
    }

#define fwCoreInterfaceIsTypeOfMacro(_classinfo_)                     \
    __FWCOREISTYPEOFMACRO(_classinfo_)                                \
    virtual bool isA(const std::string& type) const                   \
    {                                                                 \
        return __FWCORE_TYPEDEF_SELF_NAME::isTypeOf(type);            \
    }

/**
 * @brief Generate methods for classes with one factory
 *
 * For this macro parameters, each bracket is significant.
 * @param \_classinfo_ Class information in the form : (classname)(baseclassname) or (classname).
 *                  baseclassname is only required for a non-base class, and should not
 *                  be used if baseclassname == classname
 *
 * @param \_parameters_ ( types_param_factory0 ) (...) ( types_param_factoryN )
 *                      - where types_param_factoryX can be :
 *                        - for a 0-arg factory : ()
 *                        - for a 1-arg of T type factory : ((T)) or ((T)(d))
 *                          where d is a default value
 *                        - for a N-parameters factory : ((type0)) ((type1)) ... ((typeN)(default_value))
 *                      - Several types_param_factory can be declared if a factory have several signatures
 *                        - Example for several signatures : () ( ((int)) ) ( ((int)) ((std::string)("default")) )
 *
 * @param \_factory_ A factory that can take arguments as defined in _parameters_ arguments
 */
#if !BOOST_PP_VARIADICS_MSVC
    #define fwCoreClassFactoryMacro(...) BOOST_PP_OVERLOAD(fwCoreClassFactoryMacro_, __VA_ARGS__)(__VA_ARGS__)
#else
    #define fwCoreClassFactoryMacro(...) \
    BOOST_PP_CAT(BOOST_PP_OVERLOAD(fwCoreClassFactoryMacro_, __VA_ARGS__)(__VA_ARGS__), BOOST_PP_EMPTY())
#endif

#define fwCoreClassFactoryMacro_2(_classinfo_, _factory_)                                               \
    __FWCORE_CLASS_TYPEDEFS(_classinfo_)                                                                \
    /** Specialized version of shared_ptr (alias to shared_ptr< __FWCORE_GET_CLASSNAME(_classinfo_) > ) \
     * with embeded factory for __FWCORE_GET_CLASSNAME(_classinfo_). */                                 \
    /* @cond */                                                                                         \
    /* @endcond */                                                                                      \
    static __FWCORE_TYPEDEF_SHARED_PTR_NAME __FWCORE_FACTORY_NAME()                                     \
    {                                                                                                   \
        return __FWCORE_TYPEDEF_SHARED_PTR_NAME(_factory_());                                           \
    }

#define fwCoreClassFactoryMacro_3(_classinfo_, _parameters_, _factory_)                                 \
    __FWCORE_CLASS_TYPEDEFS(_classinfo_)                                                                \
    /** Specialized version of shared_ptr (alias to shared_ptr< __FWCORE_GET_CLASSNAME(_classinfo_) > ) \
     * with embeded factory for __FWCORE_GET_CLASSNAME(_classinfo_). */                                 \
    /* @cond */                                                                                         \
    /* @endcond */                                                                                      \
    __FWCORE_GENERATE_FACTORIES_WITH_ONE_FACTORY(_factory_, _parameters_)

/**
 * @brief Generate common construction methods for classes with one factory
 *
 * For this macro parameters, each bracket is significant.
 * @param \_classinfo_ Class information in the form : (classname)(baseclassname) or (classname).
 *                  baseclassname is only required for a non-base class, and should not
 *                  be used if baseclassname == classname
 *
 * @param \_parameters_ ( types_param_factory0 ) (...) ( types_param_factoryN )
 *                      - where types_param_factoryX can be :
 *                        - for a 0-arg factory : ()
 *                        - for a 1-arg of T type factory : ((T)) or ((T)(d))
 *                          where d is a default value
 *                        - for a N-parameters factory : ((type0)) ((type1)) ... ((typeN)(default_value))
 *                      - Several types_param_factory can be declared if a factory have several signatures
 *                        - Example for several signatures : () ( ((int)) ) ( ((int)) ((std::string)("default")) )
 *
 * @param \_factory_ A factory that can take arguments as defined in _parameters_ arguments
 */
#if !BOOST_PP_VARIADICS_MSVC
    #define fwCoreClassDefinitionsWithFactoryMacro(...) \
    BOOST_PP_OVERLOAD(fwCoreClassDefinitionsWithFactoryMacro_, __VA_ARGS__)(__VA_ARGS__)
#else
    #define fwCoreClassDefinitionsWithFactoryMacro(...) \
    BOOST_PP_CAT(BOOST_PP_OVERLOAD(fwCoreClassDefinitionsWithFactoryMacro_, __VA_ARGS__)(__VA_ARGS__), BOOST_PP_EMPTY())
#endif

#define fwCoreClassDefinitionsWithFactoryMacro_2(_classinfo_, _factory_)                                \
    __FWCORE_CLASS_TYPEDEFS(_classinfo_)                                                                \
    /** Specialized version of shared_ptr (alias to shared_ptr< __FWCORE_GET_CLASSNAME(_classinfo_) > ) \
     * with embeded factory for __FWCORE_GET_CLASSNAME(_classinfo_). */                                 \
    /* @cond */                                                                                         \
    /* @endcond */                                                                                      \
    static __FWCORE_TYPEDEF_SHARED_PTR_NAME __FWCORE_FACTORY_NAME()                                     \
    {                                                                                                   \
        return __FWCORE_TYPEDEF_SHARED_PTR_NAME(_factory_());                                           \
    }                                                                                                   \
    __FWCORE_GENERATE_CAST(__FWCORE_GET_CLASSNAME(_classinfo_))                                         \
    __FWCORE_CLASSNAME_MACRO()                                                                          \
    fwCoreIsTypeOfMacro(_classinfo_)

#define fwCoreClassDefinitionsWithFactoryMacro_3(_classinfo_, _parameters_, _factory_)                  \
    __FWCORE_CLASS_TYPEDEFS(_classinfo_)                                                                \
    /** Specialized version of shared_ptr (alias to shared_ptr< __FWCORE_GET_CLASSNAME(_classinfo_) > ) \
     * with embeded factory for __FWCORE_GET_CLASSNAME(_classinfo_). */                                 \
    /* @cond */                                                                                         \
    /* @endcond */                                                                                      \
    __FWCORE_GENERATE_FACTORIES_WITH_ONE_FACTORY(_factory_, _parameters_)                               \
    __FWCORE_GENERATE_CAST(__FWCORE_GET_CLASSNAME(_classinfo_))                                         \
    __FWCORE_CLASSNAME_MACRO()                                                                          \
    fwCoreIsTypeOfMacro(_classinfo_)

/**
 * @brief Generate common methods for classes without factory
 *
 * For this macro parameters, each bracket is significant.
 * @param \_classinfo_ Class information in the form : (classname)(baseclassname) or (classname).
 *                  baseclassname is only required for a non-base class, and should not
 *                  be used if baseclassname == classname
 */
#define fwCoreClassDefinitionsMacro(_classinfo_)                                                        \
    __FWCORE_CLASS_TYPEDEFS(_classinfo_)                                                                \
    __FWCORE_GENERATE_CAST(__FWCORE_GET_CLASSNAME(_classinfo_))                                         \
    __FWCORE_CLASSNAME_MACRO()                                                                          \
    fwCoreIsTypeOfMacro(_classinfo_)

/**
 * @brief Generate common code for services classes
 *
 * @param \_classinfo_ Class information in the form : (classname)(baseclassname) or (classname).
 *                  baseclassname is only required for a non-base class, and should not
 *                  be used if baseclassname == classname
 */
#define fwCoreServiceClassDefinitionsMacro(_classinfo_)                                           \
    __FWCORE_CLASS_TYPEDEFS(_classinfo_)                                                          \
    __FWCORE_GENERATE_CAST(__FWCORE_GET_CLASSNAME(_classinfo_))                                   \
    __FWCORE_CLASSNAME_MACRO()                                                                    \
    fwCoreIsTypeOfMacro(_classinfo_)

/**
 * @brief Generate common code for Non Instanciable classes (Interfaces, Abstract classes, ...)
 *

 * @param \_classinfo_ Class information in the form : (classname)(baseclassname) or (classname).
 *                  baseclassname is only required for a non-base class, and should not
 *                  be used if baseclassname == classname
 */
#define fwCoreNonInstanciableClassDefinitionsMacro(_classinfo_)                                   \
    __FWCORE_CLASS_TYPEDEFS(_classinfo_)                                                          \
    __FWCORE_GENERATE_CAST(__FWCORE_GET_CLASSNAME(_classinfo_))                                   \
    __FWCORE_CLASSNAME_MACRO()                                                                    \
    fwCoreIsTypeOfMacro(_classinfo_)

/**
 * @brief Generate common code for a base class (Interfaces, Abstract classes, ...)
 *

 * @param \_classinfo_ Class information in the form : (classname)(baseclassname) or (classname).
 *                  baseclassname is only required for a non-base class, and should not
 *                  be used if baseclassname == classname
 */
#define fwCoreBaseClassDefinitionsMacro(_classinfo_)                                              \
    __FWCORE_CLASS_TYPEDEFS(_classinfo_)                                                          \
    __FWCORE_GENERATE_CAST(__FWCORE_GET_CLASSNAME(_classinfo_))                                   \
    __FWCORE_INTERFACE_MACRO()                                                                    \
    fwCoreInterfaceIsTypeOfMacro(_classinfo_)

/**  @} */
