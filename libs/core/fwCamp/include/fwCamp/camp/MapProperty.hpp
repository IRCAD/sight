/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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
