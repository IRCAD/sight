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

#include <boost/preprocessor/array/data.hpp>
#include <boost/preprocessor/array/push_front.hpp>
#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/facilities/expand.hpp>
#include <boost/preprocessor/repetition/for.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/seq.hpp>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/seq/to_tuple.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/tuple/to_seq.hpp>

/** @cond */
#define __FWTOOLS_MEMBER_PREFIX                   m_
#define __FWTOOLS_ATTRIBUTE_ACCESSOR_PARAM_PREFIX _
#define __FWTOOLS_ATTRIBUTE_MAP_NAME              __m_object_attributes__
#define __FWTOOLS_ATTRIBUTES_REGISTER_FUNC_NAME   __registerAttributes

#define __FWTOOLS_NONE
/** @endcond */


#define fwToolsRegisterAttributeSignatureMacro()   \
    void __FWTOOLS_ATTRIBUTES_REGISTER_FUNC_NAME()


#define fwToolsSetAttributeSignatureMacro()                   \
    void setAttribute( AttrNameType attrName, AttrType _obj )

#define fwToolsPrependParamPrefixMacro( _name )                     \
    BOOST_PP_CAT (__FWTOOLS_ATTRIBUTE_ACCESSOR_PARAM_PREFIX, _name)

#define fwToolsAttributeTypeMacro( _type )  \
    _type::__FWCORE_TYPEDEF_SHARED_PTR_NAME

//-----------------------------------
// Setter
//-----------------------------------
#define fwToolsSetterMacro( _memberprefix, _type, _var) \
    fwToolsSetMacro    ( _memberprefix, _type, _var);   \
    fwToolsSetCRefMacro( _memberprefix, _type, _var);

//-----------------------------------
//
#define fwToolsSetMacro( _memberprefix, _type, _var)                                      \
    void set##_var (const _type fwToolsPrependParamPrefixMacro(_var))                     \
    {                                                                                     \
        this->BOOST_PP_CAT( _memberprefix, _var ) = fwToolsPrependParamPrefixMacro(_var); \
    }

//-----------------------------------
//
#define fwToolsSetCRefMacro( _memberprefix, _type, _var)                                  \
    void setCRef##_var (const _type & fwToolsPrependParamPrefixMacro(_var))               \
    {                                                                                     \
        this->BOOST_PP_CAT( _memberprefix, _var ) = fwToolsPrependParamPrefixMacro(_var); \
    }

//-----------------------------------


//-----------------------------------
// Getter
//-----------------------------------
#define fwToolsGetterMacro( _memberprefix, _type, _var) \
    fwToolsGetMacro    ( _memberprefix, _type, _var);   \
    fwToolsGetRefMacro ( _memberprefix, _type, _var);   \
    fwToolsGetCRefMacro( _memberprefix, _type, _var);


//-----------------------------------
//
#define fwToolsGetMacro( _memberprefix, _type, _var) \
    const _type get##_var () const                   \
    {                                                \
        return BOOST_PP_CAT( _memberprefix, _var );  \
    }

//-----------------------------------
//
#define fwToolsGetRefMacro( _memberprefix, _type, _var)   \
    _type & getRef##_var ()                               \
    {                                                     \
        return this->BOOST_PP_CAT( _memberprefix, _var ); \
    }

//-----------------------------------
//
#define fwToolsGetCRefMacro( _memberprefix, _type, _var)  \
    const _type & getCRef##_var () const                  \
    {                                                     \
        return this->BOOST_PP_CAT( _memberprefix, _var ); \
    }

//-----------------------------------
// Getter/Setter
//-----------------------------------
#define fwGetterSetterMacro( _memberprefix, _type, _name, _desc )                   \
    /** @name BOOST_PP_CAT( _memberprefix, _var ) accessor */                       \
    /** Getter/Setter for _var                            */                        \
    /** @{                                                */                        \
    fwToolsGetterMacro( _memberprefix, fwToolsAttributeTypeMacro( _type ) , _name); \
    fwToolsSetterMacro( _memberprefix, fwToolsAttributeTypeMacro( _type ) , _name); \
    /**@}                                                 */


#define fwToolsDeclareAttrMacro( _memberprefix, _type, _name, _desc )          \
    /** @desc _desc **/                                                        \
     fwToolsAttributeTypeMacro( _type )  BOOST_PP_CAT( _memberprefix, _name );


#define fwToolsDeclareAttrTypedefMacro( _type, _name, _desc ) \
    /** @desc _desc **/                                       \
     typedef _type BOOST_PP_CAT( _name, Type );



#define __FWTOOLS_ONE_ATTR_REGISTER_MACRO( _memberprefix, _type, _name, _desc )  \
    /* Register the specified attribute in the map */                            \
      /* Insert pair into map */                                                 \
      __FWTOOLS_ATTRIBUTE_MAP_NAME.insert(                                       \
              std::make_pair(                                                    \
                  BOOST_PP_STRINGIZE(_name),                                     \
                  boost::ref( (AttrType&) BOOST_PP_CAT( _memberprefix, _name ) ) \
                  ));

#define __FWTOOLS_SET_ONE_ATTR_MACRO(_type, _name, _desc )                                          \
    /* check the given object and Set the specified attribute value */                              \
    else if( BOOST_PP_STRINGIZE(_name) == attrName && BOOST_PP_CAT(_name,Type)::dynamicCast(_obj) ) \
    {                                                                                               \
        theAttr = _obj;                                                                             \
    }


#define __FWTOOLS_PREPEND_TO_TUPLE_MACRO( _size, _tuple, _value )           \
    BOOST_PP_ARRAY_DATA(BOOST_PP_ARRAY_PUSH_FRONT((_size, _tuple), _value))

#define fwToolsOneAttrRegisterMacro( r, _data_memberprefix, _tuple )                                  \
    BOOST_PP_EXPAND( __FWTOOLS_ONE_ATTR_REGISTER_MACRO __FWTOOLS_PREPEND_TO_TUPLE_MACRO(3, _tuple, _data_memberprefix))


#define fwToolsOneAttrDeclareMacro( r, _data_memberprefix, _tuple )                                            \
    BOOST_PP_EXPAND( fwToolsDeclareAttrMacro __FWTOOLS_PREPEND_TO_TUPLE_MACRO(3, _tuple, _data_memberprefix) )


#define fwToolsOneAttrTypedefDeclareMacro( r, data, _tuple ) \
    fwToolsDeclareAttrTypedefMacro _tuple


#define fwToolsOneAttrGetterSetterMacro( r, _data_memberprefix, _tuple )                 \
    BOOST_PP_EXPAND( fwGetterSetterMacro  __FWTOOLS_PREPEND_TO_TUPLE_MACRO(3, _tuple, _data_memberprefix))

#define fwToolsSetOneAttrMacro( r, data, _tuple ) \
    __FWTOOLS_SET_ONE_ATTR_MACRO _tuple


#define fwToolsDeclareAttributesMacro( _memberprefix, _attributes )                   \
        BOOST_PP_SEQ_FOR_EACH(fwToolsOneAttrDeclareMacro, _memberprefix, _attributes)

#define fwToolsDeclareAttributesTypedefMacro( _memberprefix, _attributes )                   \
        BOOST_PP_SEQ_FOR_EACH(fwToolsOneAttrTypedefDeclareMacro, _memberprefix, _attributes)


#define fwToolsGetterSetterAttributesMacro( _memberprefix, _attributes )                   \
    /** @cond **/                                                                          \
        BOOST_PP_SEQ_FOR_EACH(fwToolsOneAttrGetterSetterMacro, _memberprefix, _attributes) \
    /** @endcond **/


#define fwToolsRegisterAttributesMacro( _memberprefix, _attributes )                    \
    /** @cond **/                                                                       \
    fwToolsRegisterAttributeSignatureMacro()                                            \
    {                                                                                   \
        BOOST_PP_SEQ_FOR_EACH(fwToolsOneAttrRegisterMacro,  _memberprefix, _attributes) \
    }                                                                                   \
    /** @endcond **/


#define fwToolsSetAttributesMacro( _attributes )                                           \
    /** @cond **/                                                                          \
    fwToolsSetAttributeSignatureMacro()                                                    \
    {                                                                                      \
        AttrRefType::type &theAttr = this->getAttribute(attrName);                         \
        if (!_obj)                                                                         \
        {                                                                                  \
            theAttr.reset();                                                               \
        }                                                                                  \
        BOOST_PP_SEQ_FOR_EACH(fwToolsSetOneAttrMacro, _, _attributes)                      \
        else                                                                               \
        {                                                                                  \
            OSLM_FATAL( "Attribute '"<<attrName<<"' of object '"<< this->getClassname()    \
                      <<"' can not handle a object of type '"<<_obj->getClassname()<<"'"); \
        }                                                                                  \
    }
    /** @endcond **/



#define fwToolsPrefixedAttributesMacro( _prefix, _attributes )        \
    private:                                                          \
        fwToolsDeclareAttributesMacro  ( _prefix, _attributes )       \
        fwToolsRegisterAttributesMacro ( _prefix, _attributes )       \
    public:                                                           \
        fwToolsDeclareAttributesTypedefMacro ( _prefix, _attributes ) \
        fwToolsGetterSetterAttributesMacro   ( _prefix, _attributes ) \
        fwToolsSetAttributesMacro            ( _attributes )


#define fwToolsAttributesMacro( _attributes )                              \
    fwToolsPrefixedAttributesMacro( __FWTOOLS_MEMBER_PREFIX, _attributes )


#endif //_FWTOOLS_ATTRIBUTESMACROS_HPP_
