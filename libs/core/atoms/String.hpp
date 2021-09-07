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
 * @brief Class representing a String.
 *
 */
class ATOMS_CLASS_API String : public Base
{
public:

    SIGHT_DECLARE_CLASS(String, atoms::Base, atoms::factory::New<String>);

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    String(atoms::Base::Key)
    {
    }

    /**
     * @brief   Destructor
     */
    virtual ~String()
    {
    }

    //------------------------------------------------------------------------------

    const std::string& getValue() const
    {
        return m_value;
    }

    //------------------------------------------------------------------------------

    void setValue(const std::string& value)
    {
        m_value = value;
    }

    //------------------------------------------------------------------------------

    std::string getString() const override
    {
        return m_value;
    }

    /**
     * @brief Construct a new Object represented a string.
     * @param value the internal string.
     *
     */
    ATOMS_API static String::sptr New(std::string value);

    /**
     * @brief Returns a clone object
     */
    ATOMS_API Base::sptr clone() const override;

    /**
     * @brief returns Atom type
     */
    atoms::Base::AtomType type() const override
    {
        return atoms::Base::STRING;
    }

protected:

    std::string m_value;
};

}
