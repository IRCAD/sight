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
 * @brief This file defines fwCore base macros.
 */

#pragma once

#include "fwCore/Demangler.hpp"
#include "fwCore/macros-legacy.hpp"

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/facilities/overload.hpp>

#include <memory>
#include <string>
#include <type_traits>

#define FwCoreNotUsedMacro(x) ((void) x )

/**
 * @name Smart pointers macro
 * @{ */
// Expand to shared_ptr < _cls_ >
#define SPTR(_cls_)  std::shared_ptr < _cls_ >
// Expand to shared_ptr < const _cls_ >
#define CSPTR(_cls_) std::shared_ptr < const _cls_ >
// Expand to weak_ptr < _cls_ >
#define WPTR(_cls_)  std::weak_ptr < _cls_ >
// Expand to weak_ptr < const _cls_ >
#define CWPTR(_cls_) std::weak_ptr < const _cls_ >
// Expand to unique_ptr < _cls_ >
#define UPTR(_cls_)  std::unique_ptr < _cls_ >
// Expand to unique_ptr < const _cls_ >
#define CUPTR(_cls_) std::unique_ptr < const _cls_ >
/**  @} */

/*
 * @brief Define several typdefs for classes (sptr, wptr, ...)
 */
#define __FWCORE_CLASS_TYPEDEFS_1(_class)      \
    /** Self type  */                          \
    typedef _class SelfType;                   \
    /** Type of base class  */                 \
    typedef SelfType BaseClass;                \
    /** Type of root class  */                 \
    typedef SelfType RootClass;                \
    /** Shared pointer type  */                \
    typedef SPTR ( SelfType ) sptr;            \
    /** Weak pointer type  */                  \
    typedef WPTR ( SelfType ) wptr;            \
    /** Unique pointer type  */                \
    typedef UPTR ( SelfType ) uptr;            \
    /** Const shared pointer type  */          \
    typedef CSPTR ( SelfType ) csptr;          \
    /** Const weak pointer type  */            \
    typedef CWPTR ( SelfType ) cwptr;          \
    /** Const unique pointer type  */          \
    typedef CUPTR ( SelfType ) cuptr;          \

/**
 * @brief Define several typdefs for classes (sptr, wptr, ...)
 *
 * BaseClass is a typedef to the superclass
 */
#define __FWCORE_CLASS_TYPEDEFS_2(_class, _parentClass) \
    /** Self type  */                                   \
    typedef _class SelfType;                            \
    /** Type of base class  */                          \
    typedef  _parentClass BaseClass;                    \
    /** Type of root class  */                          \
    typedef BaseClass::RootClass RootClass;             \
    /** Shared pointer type  */                         \
    typedef SPTR ( SelfType ) sptr;                     \
    /** Weak pointer type  */                           \
    typedef WPTR ( SelfType ) wptr;                     \
    /** Unique pointer type  */                         \
    typedef UPTR ( SelfType ) uptr;                     \
    /** Const shared pointer type  */                   \
    typedef CSPTR ( SelfType ) csptr;                   \
    /** Const weak pointer type  */                     \
    typedef CWPTR ( SelfType ) cwptr;                   \
    /** Const unique pointer type  */                   \
    typedef CUPTR ( SelfType ) cuptr;

/**
 * @brief Cast definition for casting from baseclassname and derived to _classname_
 */
#define __FWCORE_GENERATE_CAST(_classname_)                                                                         \
    /** @brief Cast to dynamic shared pointer   */                                                                  \
    template< class BASETYPE, typename = typename std::enable_if < std::is_const<BASETYPE>::value >::type >         \
    static std::shared_ptr< const _classname_> dynamicCast(const std::shared_ptr<BASETYPE> &p )                     \
    {                                                                                                               \
        return std::dynamic_pointer_cast< const _classname_ >(p);                                                   \
    }                                                                                                               \
    template< class BASETYPE, typename = typename std::enable_if < !std::is_const<BASETYPE>::value >::type >        \
    static std::shared_ptr<_classname_> dynamicCast(const std::shared_ptr<BASETYPE> &p )                            \
    {                                                                                                               \
        return std::dynamic_pointer_cast< _classname_ >(p);                                                         \
    }                                                                                                               \
    /** @brief Const shared pointer cast to dynamic pointer \
     *  @deprecated simply use dynamicCast instead, now it handles const or not const arguments. \
     */                                                                                                             \
    template< class BASETYPE > static csptr dynamicConstCast( BASETYPE const &p )                                   \
    {                                                                                                               \
        return std::dynamic_pointer_cast< const _classname_ >(p);                                                   \
    }                                                                                                               \
    /** @brief Cast to const shared pointer */                                                                      \
    static sptr constCast(csptr const &p )                                                                          \
    {                                                                                                               \
        return std::const_pointer_cast< _classname_ >(p);                                                           \
    }

#define __FWCORE_STATIC_CACHE( value )         \
    static const std::string __cache__(value); \
    return __cache__;

/**
 * @brief Generate virtual methods that return classname/namespace strings
 *
 * Example: for ::fwData::Object,
 * - Classname is ::fwData::Object
 * - LeafClassname is Object
 */
#define __FWCORE_INTERFACE_MACRO()                                                              \
    /** @name Demangling methods */                                                             \
    /** @{ */                                                                                   \
    /** @brief return object's classname without its namespace, i.e. BaseObject */              \
    virtual const std::string& getLeafClassname() const                                         \
    {                                                                                           \
        __FWCORE_STATIC_CACHE(::fwCore::Demangler(*this).getLeafClassname());                   \
    }                                                                                           \
    static const std::string& leafClassname()                                                   \
    {                                                                                           \
        __FWCORE_STATIC_CACHE(::fwCore::getLeafClassname<SelfType>());                          \
    }                                                                                           \
    /** @brief return full object's classname with its namespace, i.e. ::fwCore::BaseObject */  \
    virtual const std::string& getClassname() const                                             \
    {                                                                                           \
        __FWCORE_STATIC_CACHE(::fwCore::getClassname<SelfType>());                              \
    }                                                                                           \
    static const std::string& classname()                                                       \
    {                                                                                           \
        __FWCORE_STATIC_CACHE(::fwCore::getClassname<SelfType>());                              \
    }                                                                                           \
    /** @} */

/**
 * @brief Generate virtual methods that return classname/namespace strings
 *
 * Example: for ::fwData::Object,
 * - Classname is ::fwData::Object
 * - LeafClassname is Object
 */
#define __FWCORE_CLASSNAME_MACRO()                                                              \
    /** @name Demangling methods */                                                             \
    /** @{ */                                                                                   \
    /** @brief return object's classname without its namespace, i.e. BaseObject */              \
    virtual const std::string& getLeafClassname() const override                                \
    {                                                                                           \
        __FWCORE_STATIC_CACHE(::fwCore::Demangler(*this).getLeafClassname());                   \
    }                                                                                           \
    static const std::string& leafClassname()                                                   \
    {                                                                                           \
        __FWCORE_STATIC_CACHE(::fwCore::getLeafClassname<SelfType>());                          \
    }                                                                                           \
    /** @brief return full object's classname with its namespace, i.e. ::fwCore::BaseObject */  \
    virtual const std::string& getClassname() const override                                    \
    {                                                                                           \
        __FWCORE_STATIC_CACHE(::fwCore::getClassname<SelfType>());                              \
    }                                                                                           \
    static const std::string& classname()                                                       \
    {                                                                                           \
        __FWCORE_STATIC_CACHE(::fwCore::getClassname<SelfType>());                              \
    }                                                                                           \
    /** @} */

/**
 * @brief Generate virtual methods that check if passed type is same type of
 * (or a topclass of) 'this' type
 *
 * Example:
 * ::fwData::Image::IsTypeOf("::fwData::Object") is true
 * image->isA("::fwData::Object") is true
 *
 */
#define __FWCORE_TYPE_1(_class)                      \
    static bool isTypeOf(const std::string& type)    \
    {                                                \
        return (SelfType::classname() == type);      \
    }                                                \
    virtual bool isA(const std::string& type) const  \
    {                                                \
        return SelfType::isTypeOf(type);             \
    }

#define __FWCORE_TYPE_2(_class, _parentClass)                 \
    static bool isTypeOf(const std::string& type)             \
    {                                                         \
        if (SelfType::classname() == type)                    \
        {                                                     \
            return true;                                      \
        }                                                     \
        else                                                  \
        {                                                     \
            return BaseClass::isTypeOf( type );               \
        }                                                     \
    }                                                         \
    virtual bool isA(const std::string& type) const override  \
    {                                                         \
        return SelfType::isTypeOf(type);                      \
    }

/**
 * @brief Generate getSptr and getConstSptr methods
 *
 * These methods use 'shared_from_this' to get a shared pointer and cast it to required type
 */
#define fwCoreAllowSharedFromThis()                                              \
    /** @brief return a casted const shared ptr from this object */              \
    csptr getConstSptr() const                                                   \
    {                                                                            \
        return dynamicConstCast(this->::fwCore::BaseObject::shared_from_this()); \
    }                                                                            \
    /** @brief return a casted shared ptr from this object */                    \
    sptr getSptr()                                                               \
    {                                                                            \
        return dynamicCast(this->::fwCore::BaseObject::shared_from_this());      \
    }

#if !BOOST_PP_VARIADICS_MSVC
    #define fwCoreClassMacro(...) \
    BOOST_PP_OVERLOAD(__FWCORE_CLASS_MACRO_, __VA_ARGS__)(__VA_ARGS__)
#else
    #define fwCoreClassMacro(...) \
    BOOST_PP_CAT(BOOST_PP_OVERLOAD(__FWCORE_CLASS_MACRO_, __VA_ARGS__)(__VA_ARGS__), BOOST_PP_EMPTY())
#endif

#define __FWCORE_CLASS_MACRO_1(_class)  \
    __FWCORE_CLASS_TYPEDEFS_1(_class)   \
    __FWCORE_GENERATE_CAST(_class)      \
    __FWCORE_INTERFACE_MACRO()          \
    __FWCORE_TYPE_1(_class)

#define __FWCORE_CLASS_MACRO_2(_class, _parentClass) \
    __FWCORE_CLASS_TYPEDEFS_2(_class, _parentClass)  \
    __FWCORE_GENERATE_CAST(_class)                   \
    __FWCORE_CLASSNAME_MACRO()                       \
    __FWCORE_TYPE_2(_class, _parentClass)

#define __FWCORE_CLASS_MACRO_3(_class, _parentClass, _factory)  \
    __FWCORE_CLASS_MACRO_2(_class, _parentClass)                \
    static sptr New()                                           \
    {                                                           \
        return sptr(_factory());                                \
    }

/**
 * @brief Generate common code for services classes
 */
#define fwCoreServiceMacro(_class, _parentClass) \
    __FWCORE_CLASS_MACRO_2(_class, _parentClass)
