/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_MACROS_HPP__
#define __FWDATA_MACROS_HPP__

#include <boost/preprocessor/cat.hpp>

#include <fwCore/macros.hpp>
/**
 * @file fwData/macros.hpp
 * @brief fwData specific macros.
 *
 * @author IRCAD (Research and Development Team).
 */



/*
 * macros used for getters/setters
 *
 * funcName : name of the function ("Firstname" for getFirstname() function)
 * var : name of the variable without "m_" ("firstname" for m_firstname variable)
 * type : type of the variable (float, std::string, ...)
 * text : comment used in doxygen
 */



//-----------------------------------
// Getter/Setter macros
//-----------------------------------
#define __FWDATA_CLASS_ATTRIBUTE_PREFIX m_
#define __FWDATA_GENERATED_ATTRIBUTE_PREFIX attr

//-----------------------------------

#define fwDataGetMacro(attrName, attrType)                                        \
attrType get##attrName () const                                                   \
{                                                                                 \
    return BOOST_PP_CAT(                                                          \
                       __FWDATA_CLASS_ATTRIBUTE_PREFIX,                           \
                       BOOST_PP_CAT(__FWDATA_GENERATED_ATTRIBUTE_PREFIX,attrName) \
                       );                                                         \
}

#define fwDataSetMacro(attrName, attrType)                                 \
void set##attrName (attrType val)                                          \
{                                                                          \
    BOOST_PP_CAT(                                                          \
                __FWDATA_CLASS_ATTRIBUTE_PREFIX,                           \
                BOOST_PP_CAT(__FWDATA_GENERATED_ATTRIBUTE_PREFIX,attrName) \
                ) = val;                                                   \
}

#define fwDataGetSptrMacro(attrName, attrType)                                       \
attrType get##attrName () const                                                      \
{                                                                                    \
    return BOOST_PP_CAT(                                                             \
                        __FWDATA_CLASS_ATTRIBUTE_PREFIX,                             \
                        BOOST_PP_CAT( __FWDATA_GENERATED_ATTRIBUTE_PREFIX, attrName) \
                        );                                                           \
}

#define fwDataSetSptrMacro(attrName, attrType)                               \
void set##attrName (attrType val)                                            \
{                                                                            \
    BOOST_PP_CAT(                                                            \
                __FWDATA_CLASS_ATTRIBUTE_PREFIX,                             \
                BOOST_PP_CAT( __FWDATA_GENERATED_ATTRIBUTE_PREFIX, attrName) \
                ) = val;                                                     \
}

//-----------------------------------

#define fwDataGetCRefMacro(attrName, attrType)                                      \
const attrType &get##attrName () const                                              \
{                                                                                   \
    return BOOST_PP_CAT(                                                            \
                        __FWDATA_CLASS_ATTRIBUTE_PREFIX,                            \
                        BOOST_PP_CAT(__FWDATA_GENERATED_ATTRIBUTE_PREFIX, attrName) \
                        );                                                          \
}

#define fwDataSetCRefMacro(attrName, attrType)                              \
void set##attrName (const attrType &val)                                    \
{                                                                           \
    BOOST_PP_CAT(                                                           \
                __FWDATA_CLASS_ATTRIBUTE_PREFIX,                            \
                BOOST_PP_CAT(__FWDATA_GENERATED_ATTRIBUTE_PREFIX, attrName) \
                ) = val;                                                    \
}

//-----------------------------------

#define fwDataGetSetMacro(attrName, attrType) \
    /** @{ */                                 \
    fwDataGetMacro(attrName, attrType)        \
    fwDataSetMacro(attrName, attrType)        \
    /** @}  */

#define fwDataGetSetSptrMacro(attrName, attrType) \
    /** @{ */                                     \
    fwDataGetSptrMacro(attrName, attrType)        \
    fwDataSetSptrMacro(attrName, attrType)        \
    /** @}  */

#define fwDataGetSetCRefMacro(attrName, attrType) \
    /** @{ */                                     \
    fwDataGetCRefMacro(attrName, attrType)        \
    fwDataSetCRefMacro(attrName, attrType)        \
    /** @}  */



//-----------------------------------
// Old Getter/Setter macros
//-----------------------------------

#define fwGettersSettersMacro(funcName, var, type) \
    /** @name funcName accessor      */            \
    /** @{                           */            \
    fwGettersMacro(funcName, var, type);           \
    fwSettersMacro(funcName, var, type);           \
    /**@}                            */

#define fwGettersSettersDocMacro(funcName, var, type, text ) \
    /** text                         */                      \
    /** @name funcName accessor      */                      \
    /** Get/Set text                 */                      \
    /** @{                           */                      \
    fwGettersMacro(funcName, var, type);                     \
    fwSettersMacro(funcName, var, type);                     \
    /**@}                            */

//-----------------------------------
// Setter
//-----------------------------------
#define fwSettersMacro(funcName, var, type) \
    fwSetMacro(funcName, var, type);        \
    fwSetCRefMacro(funcName, var, type);

//-----------------------------------
//
#define fwSetMacro(funcName, var, type)    \
    void set##funcName (const type _##var) \
    {                                      \
        this->m_##var = _##var;            \
    }

//-----------------------------------
//
#define fwSetCRefMacro(funcName, var, type)      \
    void setCRef##funcName (const type & _##var) \
    {                                            \
        this->m_##var = _##var;                  \
    }

//-----------------------------------


//-----------------------------------
// Getter
//-----------------------------------
#define fwGettersMacro(funcName, var, type) \
    fwGetMacro(funcName, var, type);        \
    fwGetRefMacro(funcName, var, type);     \
    fwGetCRefMacro(funcName, var, type);


//-----------------------------------
//
#define fwGetMacro(funcName, var, type) \
    const type get##funcName () const   \
    {                                   \
        return m_##var;                 \
    }

//-----------------------------------
//
#define fwGetRefMacro(funcName, var, type) \
    type & getRef##funcName ()             \
    {                                      \
        return this->m_##var;              \
    }

//-----------------------------------
//
#define fwGetCRefMacro(funcName, var, type) \
    const type & getCRef##funcName () const \
    {                                       \
        return this->m_##var;               \
    }



//-----------------------------------
// Deep/Shallow copy
//-----------------------------------

#define fwDataShallowCopyMacro()                                                \
    void shallowCopy( ::fwData::Object::csptr _source )                         \
    {                                                                           \
        ::fwData::Object::shallowCopy< __FWCORE_TYPEDEF_SELF_NAME >( _source ); \
    };                                                                          \
    void shallowCopy( __FWCORE_TYPEDEF_SELF_NAME::sptr _source )                \
    {                                                                           \
        ::fwData::Object::shallowCopy< __FWCORE_TYPEDEF_SELF_NAME >( _source ); \
    };

//-----------------------------------

#define fwDataDeepCopyMacro()                                                \
    void deepCopy( ::fwData::Object::csptr _source )                         \
    {                                                                        \
        ::fwData::Object::deepCopy< __FWCORE_TYPEDEF_SELF_NAME >( _source ); \
    };                                                                       \
    void deepCopy( __FWCORE_TYPEDEF_SELF_NAME::sptr _source )                \
    {                                                                        \
        ::fwData::Object::deepCopy< __FWCORE_TYPEDEF_SELF_NAME >( _source ); \
    };


//-----------------------------------
// General macros
//-----------------------------------
#define fwDataObjectMacro()                     \
    /** @name Deep and shallow copy methods  */ \
    /** @{                                   */ \
    fwDataShallowCopyMacro();                   \
    fwDataDeepCopyMacro();                      \
    /**@}                                    */

#endif //__FWDATA_MACROS_HPP__
