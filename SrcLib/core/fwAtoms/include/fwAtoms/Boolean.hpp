/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

#include "fwAtoms/Base.hpp"
#include "fwAtoms/config.hpp"
#include "fwAtoms/factory/new.hpp"

#include <string>

namespace fwAtoms
{

/**
 * @brief Represented a Boolean value
 *
 */
class FWATOMS_CLASS_API Boolean : public Base
{
public:
    fwCoreClassMacro(Boolean, ::fwAtoms::Base, ::fwAtoms::factory::New< Boolean >);

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    Boolean(::fwAtoms::Base::Key) :
        m_value(false)
    {
    }

    /**
     * @brief   Destructor
     */
    virtual ~Boolean()
    {
    }

    FWATOMS_API virtual std::string getString() const override;

    FWATOMS_API virtual void setString(const std::string& value);

    /**
     * @brief Construct an object storing a bool value
     * @param value a string representing a boolean.
     *
     * If value = "true" then true is stored otherwise false is stored.
     */
    FWATOMS_API static Boolean::sptr New(std::string value);

    /**
     * @brief Construct an object storing a bool value
     * @param value the boolean value.
     */
    FWATOMS_API static Boolean::sptr New(bool value);

    /**
     * @brief Returns a clone object
     */
    FWATOMS_API virtual Base::sptr clone() const override;

    /**
     * @brief returns Atom type
     */
    ::fwAtoms::Base::AtomType type() const override
    {
        return ::fwAtoms::Base::BOOLEAN;
    }

    //------------------------------------------------------------------------------

    bool getValue() const
    {
        return m_value;
    }

protected:

    /**
     * @brief Construct an object storing a bool value
     * @param value the boolena value.
     */
    FWATOMS_API Boolean(bool value);

    /**
     * @brief Construct an object storing a bool value
     * @param value a string representing a boolean.
     *
     * If value = "true" then true is stored otherwise false is stored.
     */
    FWATOMS_API Boolean(std::string& value);

    bool m_value;
};

}
