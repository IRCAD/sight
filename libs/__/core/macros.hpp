/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "core/demangler.hpp"

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/facilities/empty.hpp>
#include <boost/preprocessor/facilities/overload.hpp>

#include <memory>
#include <string>
#include <type_traits>

#define SIGHT_NOT_USED(x) ((void) x)

/**
 * @name Smart pointers macro
 * @{ */
// Expand to shared_ptr < _cls_ >
#define SPTR(_cls_) std::shared_ptr<_cls_>
// Expand to shared_ptr < const _cls_ >
#define CSPTR(_cls_) std::shared_ptr<const _cls_>
// Expand to weak_ptr < _cls_ >
#define WPTR(_cls_) std::weak_ptr<_cls_>
// Expand to weak_ptr < const _cls_ >
#define CWPTR(_cls_) std::weak_ptr<const _cls_>
// Expand to unique_ptr < _cls_ >
#define UPTR(_cls_) std::unique_ptr<_cls_>
// Expand to unique_ptr < const _cls_ >
#define CUPTR(_cls_) std::unique_ptr<const _cls_>
/**  @} */

/*
 * @brief Define several typdefs for classes (sptr, wptr, ...)
 */
#define __FWCORE_CLASS_TYPEDEFS_1(_class) \
    /** Self type  */ \
    typedef _class self_type; \
    /** Type of base class  */ \
    typedef self_type base_class; \
    /** Type of root class  */ \
    typedef self_type root_class; \
    /** Shared pointer type  */ \
    typedef SPTR(self_type) sptr; \
    /** Weak pointer type  */ \
    typedef WPTR(self_type) wptr; \
    /** Unique pointer type  */ \
    typedef UPTR(self_type) uptr; \
    /** Const shared pointer type  */ \
    typedef CSPTR(self_type) csptr; \
    /** Const weak pointer type  */ \
    typedef CWPTR(self_type) cwptr; \
    /** Const unique pointer type  */ \
    typedef CUPTR(self_type) cuptr; \

/**
 * @brief Define several typdefs for classes (sptr, wptr, ...)
 *
 * base_class is a typedef to the superclass
 */
#define __FWCORE_CLASS_TYPEDEFS_2(_class, _parent_class) \
    /** Self type  */ \
    typedef _class self_type; \
    /** Type of base class  */ \
    typedef  _parent_class base_class; \
    /** Type of root class  */ \
    typedef base_class::root_class root_class; \
    /** Shared pointer type  */ \
    typedef SPTR(self_type) sptr; \
    /** Weak pointer type  */ \
    typedef WPTR(self_type) wptr; \
    /** Unique pointer type  */ \
    typedef UPTR(self_type) uptr; \
    /** Const shared pointer type  */ \
    typedef CSPTR(self_type) csptr; \
    /** Const weak pointer type  */ \
    typedef CWPTR(self_type) cwptr; \
    /** Const unique pointer type  */ \
    typedef CUPTR(self_type) cuptr;

#define __FWCORE_STATIC_CACHE(value) \
    static const std::string __cache__(value); \
    return __cache__;

/**
 * @brief Generate virtual methods that return classname/namespace strings
 *
 * Example: for data::object,
 * - Classname is data::object
 * - LeafClassname is Object
 */
#define __FWCORE_INTERFACE_MACRO() \
    /** @name Demangling methods */ \
    /** @{ */ \
    /** @brief return object's classname without its namespace, i.e. base_object */ \
    virtual const std::string& get_leaf_classname() const \
    { \
        __FWCORE_STATIC_CACHE(sight::core::demangler(*this).get_leaf_classname()); \
    } \
    static const std::string& leaf_classname() \
    { \
        __FWCORE_STATIC_CACHE(sight::core::get_leaf_classname<self_type>()); \
    } \
    /** @brief return full object's classname with its namespace, i.e. core::base_object */ \
    virtual const std::string& get_classname() const \
    { \
        __FWCORE_STATIC_CACHE(sight::core::get_classname<self_type>()); \
    } \
    static const std::string& classname() \
    { \
        __FWCORE_STATIC_CACHE(sight::core::get_classname<self_type>()); \
    } \
    /** @} */

/**
 * @brief Generate virtual methods that return classname/namespace strings
 *
 * Example: for data::object,
 * - Classname is data::object
 * - LeafClassname is Object
 */
#define __FWCORE_CLASSNAME_MACRO() \
    /** @name Demangling methods */ \
    /** @{ */ \
    /** @brief return object's classname without its namespace, i.e. base_object */ \
    const std::string& get_leaf_classname() const override \
    { \
        __FWCORE_STATIC_CACHE(sight::core::demangler(*this).get_leaf_classname()); \
    } \
    static const std::string& leaf_classname() \
    { \
        __FWCORE_STATIC_CACHE(sight::core::get_leaf_classname<self_type>()); \
    } \
    /** @brief return full object's classname with its namespace, i.e. core::base_object */ \
    const std::string& get_classname() const override \
    { \
        __FWCORE_STATIC_CACHE(sight::core::get_classname<self_type>()); \
    } \
    static const std::string& classname() \
    { \
        __FWCORE_STATIC_CACHE(sight::core::get_classname<self_type>()); \
    } \
    /** @} */

/**
 * @brief Generate virtual methods that check if passed type is same type of
 * (or a topclass of) 'this' type
 *
 * Example:
 * data::image::IsTypeOf("data::object") is true
 * image->is_a("data::object") is true
 *
 */
#define __FWCORE_TYPE_1(_class) \
    static bool is_type_of(const std::string& type) \
    { \
        return self_type::classname() == type; \
    } \
    virtual bool is_a(const std::string& type) const \
    { \
        return self_type::is_type_of(type); \
    }

#define __FWCORE_TYPE_2(_class, _parent_class) \
    static bool is_type_of(const std::string& type) \
    { \
        if(self_type::classname() == type) \
        { \
            return true; \
        } \
        return base_class::is_type_of(type); \
    } \
    bool is_a(const std::string& type) const override \
    { \
        return self_type::is_type_of(type); \
    }

/**
 * @brief Generate get_sptr and get_const_sptr methods
 *
 * These methods use 'shared_from_this' to get a shared pointer and cast it to required type
 */
#define SIGHT_ALLOW_SHARED_FROM_THIS() \
    /** @brief return a casted const shared ptr from this object */ \
    csptr get_const_sptr() const \
    { \
        return dynamic_pointer_cast<const self_type>(this->core::base_object::shared_from_this()); \
    } \
    /** @brief return a casted shared ptr from this object */ \
    sptr get_sptr() \
    { \
        return dynamic_pointer_cast<self_type>(this->core::base_object::shared_from_this()); \
    }

#if !BOOST_PP_VARIADICS_MSVC
    #define SIGHT_DECLARE_CLASS(...) \
    BOOST_PP_OVERLOAD(__FWCORE_CLASS_MACRO_, __VA_ARGS__)(__VA_ARGS__)
#else
    #define SIGHT_DECLARE_CLASS(...) \
    BOOST_PP_CAT(BOOST_PP_OVERLOAD(__FWCORE_CLASS_MACRO_, __VA_ARGS__)(__VA_ARGS__), BOOST_PP_EMPTY())
#endif

#define __FWCORE_CLASS_MACRO_1(_class) \
    __FWCORE_CLASS_TYPEDEFS_1(_class) \
    __FWCORE_INTERFACE_MACRO() \
    __FWCORE_TYPE_1(_class)

#define __FWCORE_CLASS_MACRO_2(_class, _parent_class) \
    __FWCORE_CLASS_TYPEDEFS_2(_class, _parent_class) \
    __FWCORE_CLASSNAME_MACRO() \
    __FWCORE_TYPE_2(_class, _parent_class)

#define __FWCORE_CLASS_MACRO_3(_class, _parentClass, _factory) \
    __FWCORE_CLASS_MACRO_2(_class, _parentClass) \
    static sptr make() \
    { \
        return _factory(); \
    }

/**
 * @brief Generate common code for services classes
 */
#define SIGHT_DECLARE_SERVICE(_class, _parent_class) \
    __FWCORE_CLASS_MACRO_2(_class, _parent_class)

/// Force inline
#ifdef _MSC_VER
    #define FINLINE __forceinline
#else
    #define FINLINE __attribute__((always_inline))
#endif

namespace sight
{

template<typename T, typename deleter = std::default_delete<T> >
using uptr = std::unique_ptr<T, deleter>;

template<typename T, typename deleter = std::default_delete<T> >
using cuptr = std::unique_ptr<const T, deleter>;

template<typename T>
using sptr = std::shared_ptr<T>;

template<typename T>
using csptr = std::shared_ptr<const T>;

template<typename T>
using wptr = std::weak_ptr<T>;

template<typename T>
using cwptr = std::weak_ptr<const T>;

} // namespace sight
