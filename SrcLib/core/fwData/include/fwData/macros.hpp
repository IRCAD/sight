/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/*
 * macros used for getters/setters
 *
 * funcName : name of the function ("Firstname" for getFirstname() function)
 * var : name of the variable without "m_" ("firstname" for m_firstname variable)
 * type : type of the variable (float, std::string, ...)
 * text : comment used in doxygen
 */

#define fwGettersSettersMacro(funcName, var, type)    \
/** @name funcName accessor      */                   \
/** @{                           */                   \
fwGettersMacro(funcName, var, type);                  \
fwSettersMacro(funcName, var, type);                  \
/**@}                            */

#define fwGettersSettersDocMacro(funcName, var, type, text )    \
/** text                         */                             \
/** @name funcName accessor      */                             \
/** Get/Set text                 */                             \
/** @{                           */                             \
fwGettersMacro(funcName, var, type);                            \
fwSettersMacro(funcName, var, type);                            \
/**@}                            */

//-----------------------------------
// Setter
//-----------------------------------
#define fwSettersMacro(funcName, var, type) \
fwSetMacro(funcName, var, type);            \
fwSetCRefMacro(funcName, var, type);

//-----------------------------------
#define fwSetMacro(funcName, var, type)           \
FWDATA_API void set##funcName (const type _##var) \
{                                             \
        this->m_##var = _##var;                   \
}
//-----------------------------------
#define fwSetCRefMacro(funcName, var, type)              \
FWDATA_API void setCRef##funcName (const type & _##var)  \
{                                                    \
        this->m_##var = _##var;                          \
}

//-----------------------------------


//-----------------------------------
// Getter
//-----------------------------------
#define fwGettersMacro(funcName, var, type)  \
fwGetMacro(funcName, var, type);             \
fwGetRefMacro(funcName, var, type);          \
fwGetCRefMacro(funcName, var, type);


//-----------------------------------
#define fwGetMacro(funcName, var, type)       \
FWDATA_API const type get##funcName () const  \
{                                         \
        return m_##var;                       \
}
//-----------------------------------
#define fwGetRefMacro(funcName, var, type) \
FWDATA_API type & getRef##funcName ()      \
{                                      \
        return this->m_##var;              \
}
//-----------------------------------
#define fwGetCRefMacro(funcName, var, type)        \
FWDATA_API const type & getCRef##funcName () const \
{                                              \
        return this->m_##var;                      \
}
//-----------------------------------

