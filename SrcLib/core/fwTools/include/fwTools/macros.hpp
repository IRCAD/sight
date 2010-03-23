/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/** 
 * @file fwTools/attributesmacros.hpp
 * @brief TODO
 *
 * @author IRCAD (Research and Development Team).
 */

#ifndef _FWTOOLS_ATTRIBUTESMACROS_HPP_
#define _FWTOOLS_ATTRIBUTESMACROS_HPP_

#include <boost/ref.hpp>

#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/facilities/expand.hpp>
#include <boost/preprocessor/repetition/for.hpp>
#include <boost/preprocessor/seq/seq.hpp>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/seq/to_tuple.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/tuple/to_seq.hpp>


/** @cond */
#define __FWTOOLS_MEMBER_PREFIX                   m_
#define __FWTOOLS_ATTRIBUTE_ACCESSOR_PARAM_PREFIX _
#define __FWTOOLS_ATTRIBUTE_MAP_NAME              __m_object_attributes__
#define __FWTOOLS_ATTRIBUTE_REGISTER_FUNC_NAME    __registerAttribute

#define __FWTOOLS_NONE
/** @endcond */


#define fwToolsPrependParamPrefix( _name ) \
    BOOST_PP_CAT (__FWTOOLS_ATTRIBUTE_ACCESSOR_PARAM_PREFIX, _name)

#define fwToolsPrependMemberPrefix( _name ) \
    BOOST_PP_CAT (__FWTOOLS_MEMBER_PREFIX, _name)


//-----------------------------------
// Setter
//-----------------------------------
#define fwToolsSetterMacro(_type, _var) \
    fwToolsSetMacro(_type, _var);       \
    fwToolsSetCRefMacro(_type, _var);

//-----------------------------------
//
#define fwToolsSetMacro(_type, _var)                                                \
    FWTOOLS_API void set##_var (const _type fwToolsPrependParamPrefix(_var))        \
    {                                                                               \
        this->fwToolsPrependMemberPrefix( _var ) = fwToolsPrependParamPrefix(_var); \
    }

//-----------------------------------
//
#define fwToolsSetCRefMacro(_type, _var)                                            \
    FWTOOLS_API void setCRef##_var (const _type & fwToolsPrependParamPrefix(_var))  \
    {                                                                               \
        this->fwToolsPrependMemberPrefix( _var ) = fwToolsPrependParamPrefix(_var); \
    }

//-----------------------------------


//-----------------------------------
// Getter
//-----------------------------------
#define fwToolsGetterMacro(_type, _var) \
    fwToolsGetMacro(_type, _var);       \
    fwToolsGetRefMacro(_type, _var);    \
    fwToolsGetCRefMacro(_type, _var);


//-----------------------------------
//
#define fwToolsGetMacro(_type, _var)               \
    FWTOOLS_API const _type get##_var () const     \
    {                                              \
        return fwToolsPrependMemberPrefix( _var ); \
    }

//-----------------------------------
//
#define fwToolsGetRefMacro(_type, _var)                  \
    FWTOOLS_API _type & getRef##_var ()                  \
    {                                                    \
        return this->fwToolsPrependMemberPrefix( _var ); \
    }

//-----------------------------------
//
#define fwToolsGetCRefMacro(_type, _var)                 \
    FWTOOLS_API const _type & getCRef##_var () const     \
    {                                                    \
        return this->fwToolsPrependMemberPrefix( _var ); \
    }

//-----------------------------------
// Getter/Setter
//-----------------------------------
#define fwGetterSetterMacro( _type, _name, _desc )           \
    /** @name fwToolsPrependMemberPrefix( _var ) accessor */ \
    /** Getter/Setter for _var                            */ \
    /** @{                                                */ \
    fwToolsGetterMacro(_type, _name);                        \
    fwToolsSetterMacro(_type, _name);                        \
    /**@}                                                 */


#define fwToolsAttrMacro( _type, _name, _desc ) \
    /** @desc _desc **/                         \
    _type fwToolsPrependMemberPrefix( _name );


#define fwToolsRegisterAttrMacro(_type, _name, _desc , _id )                                    \
    /* Register the specified attribute in the map */                                           \
    /** @cond **/                                                                               \
    void BOOST_PP_CAT(__FWTOOLS_ATTRIBUTE_REGISTER_FUNC_NAME,_id)()                             \
    {                                                                                           \
      /* Insert pair into map */                                                                \
      __FWTOOLS_ATTRIBUTE_MAP_NAME.insert(                                                      \
              std::make_pair(                                                                   \
                  BOOST_PP_STRINGIZE(_name),                                                    \
                  boost::ref( (AttrType&) fwToolsPrependMemberPrefix( _name ) )                             \
                  ));                                                                           \
      /* Call the previouly defined register function if _id > 1 */                             \
      BOOST_PP_IF(                                                                              \
              BOOST_PP_DEC(_id),                                                                \
              this->BOOST_PP_CAT(__FWTOOLS_ATTRIBUTE_REGISTER_FUNC_NAME, BOOST_PP_DEC(_id))();, \
              __FWTOOLS_NONE)                                                                   \
    }                                                                                           \
    /** @endcond **/


#define fwToolsRegisterAttrCallerMacro(count)                              \
    /** @cond **/                                                          \
    void BOOST_PP_CAT(__FWTOOLS_ATTRIBUTE_REGISTER_FUNC_NAME,s)()          \
    {                                                                      \
      /* Call the first register function */                               \
      this->BOOST_PP_CAT(__FWTOOLS_ATTRIBUTE_REGISTER_FUNC_NAME, count)(); \
    }                                                                      \
    /** @endcond **/


#define __FWTOOLS_APPEND_TO_TUPLE(size, tuple, elem) \
    BOOST_PP_EXPAND(BOOST_PP_SEQ_TO_TUPLE(BOOST_PP_TUPLE_TO_SEQ(size,BOOST_PP_SEQ_HEAD(tuple))(elem)))

#define __FWTOOLS_EXPAND_ATTR_PRED(r, state) BOOST_PP_SEQ_SIZE(state)
#define __FWTOOLS_EXPAND_ATTR_OP(r, state)   BOOST_PP_SEQ_TAIL(state)

#define __FWTOOLS_EXPAND_ATTR_MACRO(r, state)                                                                         \
    private:                                                                                                          \
        BOOST_PP_EXPAND( fwToolsAttrMacro BOOST_PP_SEQ_HEAD(state) )                                                  \
        BOOST_PP_IF(                                                                                                  \
            BOOST_PP_SEQ_SIZE(state),                                                                                 \
            BOOST_PP_EXPAND( fwToolsRegisterAttrMacro __FWTOOLS_APPEND_TO_TUPLE(3, state, BOOST_PP_SEQ_SIZE(state))), \
            __FWTOOLS_NONE                                                                                            \
                )                                                                                                     \
    public:                                                                                                           \
    BOOST_PP_EXPAND( fwGetterSetterMacro BOOST_PP_SEQ_HEAD(state) )

#define fwToolsAttributesMacro( attributes )                                                                    \
    BOOST_PP_FOR(attributes, __FWTOOLS_EXPAND_ATTR_PRED, __FWTOOLS_EXPAND_ATTR_OP, __FWTOOLS_EXPAND_ATTR_MACRO) \
    fwToolsRegisterAttrCallerMacro( BOOST_PP_SEQ_SIZE(attributes) )






#endif //_FWTOOLS_ATTRIBUTESMACROS_HPP_
