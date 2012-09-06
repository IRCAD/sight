/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWMETADATA_BASE_BOOLEAN_HPP_
#define _FWMETADATA_BASE_BOOLEAN_HPP_


#include <string>

#include "fwMetaData/config.hpp"
#include "fwMetaData/Base.hpp"

fwCampAutoDeclareMacro((fwMetaData)(Boolean), FWMETADATA_API);

namespace fwMetaData
{

/**
 * @brief Represented a Boolean value
 *
 */
class FWMETADATA_CLASS_API Boolean : public Base
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Boolean)(::fwMetaData::Boolean), (()), new Boolean) ;


    FWMETADATA_API virtual bool isValue() const {return true;};
    FWMETADATA_API virtual bool isBoolean() const {return true;};
    FWMETADATA_API virtual std::string getString() const;

    FWMETADATA_API virtual void setString(const std::string& value);

    /**
     * @brief Construct an object storing a bool value
     * @param value a string representing a boolean.
     *
     * If value = "true" then true is stored otherwise false is stored.
     */
    FWMETADATA_API static Boolean::sptr New(std::string value);

    /**
     * @brief Construct an object storing a bool value
     * @param value the boolena value.
     */
    FWMETADATA_API static Boolean::sptr New(bool value);

    FWMETADATA_API virtual Base::sptr clone();
protected:

    /**
     * @brief Construct an object storing a bool value
     * @param value the boolena value.
     */
    FWMETADATA_API Boolean(bool value);

    /**
     * @brief Construct an object storing a bool value
     * @param value a string representing a boolean.
     *
     * If value = "true" then true is stored otherwise false is stored.
     */
    FWMETADATA_API Boolean(std::string& value);
    Boolean(){};
    bool m_value;
};

}



#endif /* _FWMETADATA_BASE_BOOLEAN_HPP_ */
