/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */


#ifndef _FWCORE_DEMANGLER_HPP_
#define _FWCORE_DEMANGLER_HPP_

#include <string>
#include <typeinfo>

#include "fwCore/config.hpp"

namespace fwCore
{

 /** 
  * @brief typeid, string or object name demangler.
  *
  * getClassname() is an alias for getRootedClassname()
  *
  * @warning getRootedClassname() use on fundamentals types (int, float, ...)
  * will give a bad result ( ex: ::int )
  *
  */
class FWCORE_CLASS_API Demangler
{
public:

    /** 
     * @name Constructor/Destructor
     * @{ */


    /** 
     * @brief Constructor from a typeinfo object
     * 
     * @param t Typeinfo to demangle
     */
    FWCORE_API Demangler(const std::type_info &t);

    /** 
     * @brief Constructor from a std::string
     * 
     * @param s string to demangle
     */
    FWCORE_API Demangler(const std::string &s);

    /** 
     * @brief Constructor from any type
     * 
     * @param object which type has to be demangled
     */
    template<typename T> Demangler(T &s): m_name(typeid(s).name()) {};

    /** 
     * @brief Destructor
     */
    FWCORE_API virtual ~Demangler();

    /**  @} */

    /** 
     * @brief Returns "Object" from "::some::long::namespace::Object"
     * 
     * @return demangled class name only
     */
    FWCORE_API std::string getLeafClassname();

    /** 
     * @brief Returns "some::long::namespace::Object" from "::some::long::namespace::Object"
     * 
     * @return demangled class name with namespace (without leading "::")
     */
    FWCORE_API std::string getFullClassname();

    /** 
     * @brief Like getRootedClassname(), present for backward compatibility
     * @deprecated Use getRootedClassname() instead
     * @return rooted (with leading "::") demangled class name
     */
    FWCORE_API std::string getClassname();

    /** 
     * @brief Returns "::some::long::namespace::Object" from "::some::long::namespace::Object"
     * 
     * @return rooted (with leading "::") demangled class name
     */
    FWCORE_API std::string getRootedClassname();

    /** 
     * @brief Returns "some::long::namespace" from "::some::long::namespace::Object"
     * 
     * @return demangled namespace (without leading "::")
     */
    FWCORE_API std::string getFullNamespace();

    /** 
     * @brief Returns "::some::long::namespace" from "::some::long::namespace::Object"
     * 
     * @return rooted demangled namespace (with leading "::")
     */
    FWCORE_API std::string getRootedNamespace();


protected:
    /** 
     * @brief Store the name to demangle
     */
    const std::string m_name;

    /** 
     * @brief Process the name to demangle and return the same string whatever the OS is.
     * 
     * @return demangled string
     */
    std::string demangle();
};

 


 /** 
  * @brief Type demangler.
  * Use Demangler class to get a demangled string for the type T.
  */
template <class T>
class TypeDemangler : public Demangler
{
public:
    /** 
     * @name Constructor/Destructor
     * @{ */

    /** 
     * @brief Constructor
     * @tparam T Type to demangle
     */
    TypeDemangler<T>()  : Demangler(typeid(T)) {};

    /**  @} */
};




/** 
 * @name Type demangling functions
 * @brief These functions are provided as type-demangling helpers. See 
 * Demangler for details about returned values.
 *
 * @tparam T Type to demangle
 *
 * @{ */
template <class T>
inline std::string getLeafClassname()   { return TypeDemangler<T>().getLeafClassname();   } ;
template <class T>
inline std::string getFullClassname()   { return TypeDemangler<T>().getFullClassname();   } ;
template <class T>
inline std::string getClassname()       { return TypeDemangler<T>().getClassname();       } ;
template <class T>
inline std::string getRootedClassname() { return TypeDemangler<T>().getRootedClassname(); } ;

template <class T>
inline std::string getFullNamespace()   { return TypeDemangler<T>().getFullNamespace();   } ;
template <class T>
inline std::string getRootedNamespace() { return TypeDemangler<T>().getRootedNamespace(); } ;
/**  @} */

}

#endif // _FWCORE_DEMANGLER_HPP_
