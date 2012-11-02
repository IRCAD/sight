/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWATOMS_BASE_NUMERIC_HPP_
#define _FWATOMS_BASE_NUMERIC_HPP_


#include <string>

#include <fwCamp/Mapper/ValueMapper.hpp>
#include <fwCamp/macros.hpp>

#include "fwAtoms/config.hpp"
#include "fwAtoms/Base.hpp"

fwCampAutoDeclareMacro((fwAtoms)(Numeric), FWATOMS_API);

namespace fwAtoms
{

/**
* @brief Storing a  numeric value
**/
class FWATOMS_CLASS_API Numeric : public Base
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Numeric)(::fwAtoms::Numeric), (()), new Numeric) ;

    FWATOMS_API virtual bool isNumeric() const {return true;};
    FWATOMS_API virtual bool isValue() const {return true;};
    FWATOMS_API virtual bool isSigned();
    FWATOMS_API virtual bool isReal();
    FWATOMS_API virtual std::string getString() const {return m_value;};

    FWATOMS_API virtual void setString(std::string value) { m_value = value;};

    /**
     * @brief Constructed a new numeric type
     * @param value reference of all numeric type
     * Supported tested type are (int8/16/32/64, uint8/16/32/64, float, double)
     **/
    template< typename T >
    static Numeric::sptr New(T value);

    /**
     * @brief Storing a numeric represented by a string
     * @param value this string have to be lexical castable.
     */
    FWATOMS_API static Numeric::sptr New(std::string value);


    /**
     *@brief test if two numerics are equal
     */
    FWATOMS_API bool isEqual(Numeric::sptr obj);


    /**
     * @brief test if this numeric is higher than obj
     */
    FWATOMS_API bool isHigher(Numeric::sptr obj);

    /**
     * @brief test if this numeric is lower than obj
     */
    FWATOMS_API bool isLower(Numeric::sptr obj);

    FWATOMS_API virtual Base::sptr clone();
protected:
    Numeric(){}
    template< typename T >

    Numeric(T value);

    Numeric(std::string& value);

    std::string m_value;
};

}



#include "fwAtoms/Numeric.hxx"
#endif /* _FWATOMS_BASE_NUMERIC_HPP_ */
