/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCAMP_CAMP_MAPPROPERTY_HPP__
#define __FWCAMP_CAMP_MAPPROPERTY_HPP__

#include <camp/property.hpp>
#include <camp/value.hpp>
#include <camp/userobject.hpp>
#include <camp/camptype.hpp>

#include "fwCamp/config.hpp"

namespace camp
{

class FWCAMP_CLASS_API MapProperty : public Property
{
public:

    typedef std::pair< Value, Value > ValuePair;
    typedef std::pair< UserObject, UserObject> ObjectPair;

    FWCAMP_API MapProperty(const std::string& name, Type elementType);
    /**
     * \brief Destructor
     */
    FWCAMP_API virtual ~MapProperty();

    /**
     * \brief Get the type of the map elements
     *
     * \return Type of elements
     */
    FWCAMP_API Type elementType() const;

    /**
     * \brief Do the actual reading of the value
     *
     * This function is a pure virtual which has to be implemented in derived classes.
     *
     * \param object Object
     *
     * \return Value of the property
     */
    FWCAMP_API virtual Value getValue(const UserObject& object) const;

    /**
     * \brief Do the actual writing of the value
     *
     * This function is a pure virtual which has to be implemented in derived classes.
     *
     * \param object Object
     * \param value New value to assign to the property
     */
    FWCAMP_API virtual void setValue(const UserObject& object, const Value& value) const;


    /**
     * \brief Do the actual retrieval of the size
     *
     * \param object Object
     *
     * \return Size of the map
     */
    FWCAMP_API virtual std::size_t getSize(const UserObject& object) const = 0;

    /**
     * \brief Accept the visitation of a ClassVisitor
     *
     * \param visitor Visitor to accept
     */
    FWCAMP_API virtual void accept(ClassVisitor& visitor) const;

    FWCAMP_API virtual ValuePair getElement(const UserObject& object, std::size_t index) const = 0;

    FWCAMP_API virtual void set(const UserObject& object, const Value& key, const Value& value) const = 0;

private:

    Type m_elementType; ///< Type of the individual elements of the map
};

}  // namespace camp

CAMP_TYPE(camp::MapProperty::ValuePair);

#endif /* __FWCAMP_CAMP_MAPPROPERTY_HPP__ */
