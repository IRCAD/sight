/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "atoms/Base.hpp"
#include "atoms/config.hpp"
#include "atoms/factory/new.hpp"

#include <string>

namespace sight::atoms
{

/**
 * @brief Represents a Boolean value.
 *
 */
class ATOMS_CLASS_API Boolean : public Base
{
public:

    SIGHT_DECLARE_CLASS(Boolean, atoms::Base, atoms::factory::New<Boolean>);

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    Boolean(atoms::Base::Key) :
        m_value(false)
    {
    }

    /**
     * @brief   Destructor
     */
    virtual ~Boolean()
    {
    }

    ATOMS_API std::string getString() const override;

    ATOMS_API virtual void setString(const std::string& value);

    /**
     * @brief Construct an object storing a bool value
     * @param value a string representing a boolean.
     *
     * If value = "true" then true is stored otherwise false is stored.
     */
    ATOMS_API static Boolean::sptr New(std::string value);

    /**
     * @brief Construct an object storing a bool value
     * @param value the boolean value.
     */
    ATOMS_API static Boolean::sptr New(bool value);

    /**
     * @brief Returns a clone object
     */
    ATOMS_API Base::sptr clone() const override;

    /**
     * @brief returns Atom type
     */
    atoms::Base::AtomType type() const override
    {
        return atoms::Base::BOOLEAN;
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
    ATOMS_API Boolean(bool value);

    /**
     * @brief Construct an object storing a bool value
     * @param value a string representing a boolean.
     *
     * If value = "true" then true is stored otherwise false is stored.
     */
    ATOMS_API Boolean(std::string& value);

    bool m_value;
};

}
