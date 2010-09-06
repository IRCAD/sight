/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef DYNAMICTYPE_HPP_
#define DYNAMICTYPE_HPP_

#include <string>
#include <list>
#include <stdexcept>

#include "fwTools/config.hpp"




namespace fwTools {

/**
 * @brief   Class defining an elementary C++ type aka unsigned char, signed char, .... signed long, float, double
 * @class   DynamicType
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 *
 **/
class FWTOOLS_CLASS_API DynamicType
{
public:
    /// Default constructor
    FWTOOLS_API DynamicType();

    /// Constructor by copy
    FWTOOLS_API DynamicType(const DynamicType &);

    /**
     * @brief comparison operator
     */
    FWTOOLS_API bool operator==(const DynamicType &) const;

    /**
     * @brief comparison operator
     */
    FWTOOLS_API bool operator!=(const DynamicType &) const;



    /**
     * @brief   Set DynamicType value according given template
     * @note    A BOOST_ASSERTION can be raised if TYPE is not managed by isMapping
     **/
    template< class TYPE>
    void setType() throw(std::invalid_argument);


    /**
     * @brief   Return true iff the DynamicType value represents the TYPE
     * @note    A BOOST_ASSERTION can be raised if TYPE is not managed by isMapping
     **/
    template< class TYPE>
    bool isType() const;

    /**
     * @brief   Return the sizeof of the type
     */
    FWTOOLS_API unsigned char sizeOf() const;


    /**
     * @brief Register a new type to be managed within DynamicType.
     *
     * Check are performed to ensure robsutess
     */
    template<class NEWTYPE>
    static void registerNewType(const std::string &newKey) throw(std::invalid_argument);


    /// Default destrucor : do nothing
    FWTOOLS_API virtual ~DynamicType();

    /// Return a human readable string
    FWTOOLS_API const std::string &string() const;

    /**
     * @brief Return a human readable string for type (static version no instanciation ...)
     *
     * Use a  DynamicType::string<int>();
     */
    template<class TYPE>
    static const std::string string();

protected :

    std::string m_value; // aka key
    unsigned char m_sizeof;

    /// Container of types managed by DynamicType
    FWTOOLS_API static std::list< std::string > m_managedTypes;

    /// Value for not specified type
    static const std::string m_unSpecifiedType;




};

/// Helper to create object DynamicType from a given type TYPE \in { (un)signed char, ... , double }
template<class TYPE>
DynamicType makeDynamicType();

/**
 * @brief   Helper to create object DynamicType from a given type from a KEYTYPE representing a type \in { (un)signed char, ... , double }
 *
 * Example makeDynamicType(type_id(int)) and   makeDynamicType("signed int")  should work
 */
// #include "KEYTYPEMapping.hpp" is mandatory
template<class KEYTYPE>
DynamicType makeDynamicType(const  KEYTYPE &keyType);


} //end namespace fwTools {


#include "fwTools/DynamicType.hxx"


#endif /*DYNAMICTYPE_H_*/
