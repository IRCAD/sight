/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __FWCAMP_MACROS_HPP__
#define __FWCAMP_MACROS_HPP__

#ifndef CAMP_COMPILATION

#define fwCampAutoDeclareMacro( desc, export)
#define fwCampAutoDeclareDataMacro( desc, export)
#define fwCampMakeFriendDataMacro(desc)

#else

#include <fwCore/macros.hpp>

#include <boost/preprocessor/seq/fold_left.hpp>

#include <camp/camptype.hpp>
#include <camp/class.hpp>
#include <camp/enum.hpp>

#define __FWCAMP_CAT(_s_, _state_, _elem_) BOOST_PP_CAT(_state_, _elem_)
#define __FWCAMP_NAMESPACE_CAT(_s_, _state_, _elem_) _state_::_elem_

#define __FWCAMP_FUNC_SUFFIX(desc) \
    BOOST_PP_SEQ_FOLD_LEFT( __FWCAMP_CAT, BOOST_PP_SEQ_HEAD(desc), BOOST_PP_SEQ_TAIL(desc))

#define __FWCAMP_NAMESPACE_NAME(desc) \
    ::BOOST_PP_SEQ_FOLD_LEFT( __FWCAMP_NAMESPACE_CAT, BOOST_PP_SEQ_HEAD(desc), BOOST_PP_SEQ_TAIL(desc))

#define __FWCAMP_DECLARE_FUNC_NAME(desc)        BOOST_PP_CAT(fwCampDeclare, __FWCAMP_FUNC_SUFFIX(desc))
#define __FWCAMP_DECLARE_LOCAL_FUNC_NAME(desc)  BOOST_PP_CAT(localDeclare, __FWCAMP_FUNC_SUFFIX(desc))

#define __FWCAMP_CLASS_BUILDER_TYPE(desc) \
    camp::ClassBuilder < __FWCAMP_NAMESPACE_NAME(desc) >

#define __FWCAMP_AUTO_TYPE_NONCOPYABLE(type, registerFunc) \
    CAMP_TYPE_NONCOPYABLE( type)

#define __FWCAMP__AUTO__DECLARE__MACRO(desc) \
    void __FWCAMP_DECLARE_FUNC_NAME(desc)(__FWCAMP_CLASS_BUILDER_TYPE(desc) &); \
    inline void __FWCAMP_DECLARE_LOCAL_FUNC_NAME(desc)() \
    { \
        __FWCAMP_CLASS_BUILDER_TYPE(desc) builder = \
            camp::Class::declare< __FWCAMP_NAMESPACE_NAME(desc) >(BOOST_PP_STRINGIZE( __FWCAMP_NAMESPACE_NAME( \
                                                                                          desc) )); \
        __FWCAMP_DECLARE_FUNC_NAME(desc)(builder); \
    } \
    __FWCAMP_AUTO_TYPE_NONCOPYABLE(__FWCAMP_NAMESPACE_NAME(desc), __FWCAMP_DECLARE_LOCAL_FUNC_NAME(desc))

#define __FWCAMP__REG_NAME(desc) \
    BOOST_PP_CAT(__FWCAMP_FUNC_SUFFIX(desc), Reg)

#define __FWCAMP__USEROBJREG(desc) \
    ::fwCamp::UserObjectRegistrar < __FWCAMP_NAMESPACE_NAME(desc) >
//----------------------------------------------------------------------------

/**
 *
 * desc is a BOOST_PP_SEQ
 * fwCampMakeFriendMacro((a)(b)(c)) expands to : Friend void ::fwCampDeclareabc(camp::ClassBuilder< ::a::b::c >
 *&builder)
 */
#define fwCampMakeFriendDataMacro(desc) \
    friend void ::__FWCAMP_DECLARE_FUNC_NAME(desc)(__FWCAMP_CLASS_BUILDER_TYPE(desc) &builder); \

//----------------------------------------------------------------------------

#define fwCampMakeFriendMacro(desc) \
    friend void ::__FWCAMP_DECLARE_FUNC_NAME(desc)(__FWCAMP_CLASS_BUILDER_TYPE(desc) &builder); \

//----------------------------------------------------------------------------

#define fwCampDeclareMacro(classname) \
    CAMP_TYPE_NONCOPYABLE(classname)

//----------------------------------------------------------------------------

#define fwCampAutoDeclareDataMacro( desc, export) \
    namespace fwCamp \
    { \
    template<typename T> \
    struct UserObjectRegistrar; \
    } \
    fwCorePredeclare(desc) \
    __FWCAMP__AUTO__DECLARE__MACRO(desc)

//----------------------------------------------------------------------------

#define fwCampAutoDeclareMacro( desc, export) \
    fwCorePredeclare(desc) \
    export __FWCAMP__AUTO__DECLARE__MACRO(desc)

//----------------------------------------------------------------------------

#define fwCampAutoDeclareEnumMacro(desc) \
    void __FWCAMP_DECLARE_FUNC_NAME(desc)(camp::EnumBuilder &); \
    inline void __FWCAMP_DECLARE_LOCAL_FUNC_NAME(desc)() \
    { \
        camp::EnumBuilder builder = \
            camp::Enum::declare< __FWCAMP_NAMESPACE_NAME(desc) >(BOOST_PP_STRINGIZE( __FWCAMP_NAMESPACE_NAME(desc) )); \
        __FWCAMP_DECLARE_FUNC_NAME(desc)(builder); \
    } \
    CAMP_TYPE(__FWCAMP_NAMESPACE_NAME(desc))

//----------------------------------------------------------------------------

#define fwCampImplementMacro(desc) \
    void __FWCAMP_DECLARE_FUNC_NAME(desc)(__FWCAMP_CLASS_BUILDER_TYPE(desc) &builder)

//----------------------------------------------------------------------------

#define fwCampImplementDataMacro(desc) \
    static __FWCAMP__USEROBJREG(desc) __FWCAMP__REG_NAME(desc)(BOOST_PP_STRINGIZE( __FWCAMP_NAMESPACE_NAME(desc) )); \
    void __FWCAMP_DECLARE_FUNC_NAME(desc)(__FWCAMP_CLASS_BUILDER_TYPE(desc) &builder)

//----------------------------------------------------------------------------

/**
 * @brief Implementation of enumeration into camp introspection
 * @see fwCampAutoDeclareEnumMacro
 */
#define fwCampImplementEnumMacro(desc) \
    void __FWCAMP_DECLARE_FUNC_NAME(desc)(camp::EnumBuilder& builder)

/**
 * @brief This macro is used when you want to set a smart pointer into an object
 * @param object the type of the main object ex: (fwData)(Mesh)
 * @param attribut the type of smart pointer attribut ex: (fwData)(Array)
 */
#define fwCampDeclareAccessor(object, attribut) \
    namespace camp { \
    namespace detail { \
\
    template <> \
    class Accessor1< __FWCAMP_NAMESPACE_NAME(object), SPTR( __FWCAMP_NAMESPACE_NAME(attribut) )& > \
    { \
    public: \
        typedef SPTR ( __FWCAMP_NAMESPACE_NAME (attribut) )& ReturnType; \
        typedef ObjectTraits< SPTR( __FWCAMP_NAMESPACE_NAME(attribut) )& > Traits; \
        typedef Traits::DataType DataType; \
        typedef __FWCAMP_NAMESPACE_NAME (object) ClassType; \
\
        enum \
        { \
            canRead  = true, \
            canWrite = true \
        }; \
\
        template <typename F> \
        Accessor1(F getter) :              \
            m_getter(getter) \
        { \
        } \
\
        ReturnHelper< ReturnType>::Type get(ClassType& objectptr) const \
        { \
            ReturnType ptr = m_getter(objectptr); \
            return ReturnHelper< ReturnType>::get(ptr); \
        } \
\
        bool set(ClassType& objectptr, const Value &value) const \
        { \
            ReturnType ptr = m_getter(objectptr); \
            ptr = __FWCAMP_NAMESPACE_NAME(attribut) ::dynamicCast(((value.to< DataType* >()))->getSptr()); \
            return true; \
        } \
\
    private: \
\
        boost::function< ReturnType(ClassType&)> m_getter; \
    }; \
\
    } \
    }

#endif

#endif // __FWCAMP_MACROS_HPP__

