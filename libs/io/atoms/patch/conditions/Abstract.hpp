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

#include "io/atoms/config.hpp"

#include <atoms/Base.hpp>

#include <core/base.hpp>

namespace sight::io::atoms::patch
{

namespace conditions
{

/**
 * @brief Defines a condition in the patch.
 */
class IO_ATOMS_CLASS_API Abstract : public core::BaseObject
{
public:

    SIGHT_DECLARE_CLASS(Abstract, core::BaseObject, new Abstract);
    SIGHT_ALLOW_SHARED_FROM_THIS();

    /// Default value getter
    IO_ATOMS_API virtual sight::atoms::Base::sptr getDefaultValue();

    /// Test in replacement value
    IO_ATOMS_API virtual bool test(sight::atoms::Base::sptr old, sight::atoms::Base::sptr newValue);

    /// Test with one value (remove/add/rename)
    IO_ATOMS_API virtual bool test(sight::atoms::Base::sptr old);

protected:

    Abstract()
    {
    }

    Abstract(sight::atoms::Base::sptr defaultValue);

    sight::atoms::Base::sptr m_defaultValue;
    std::string m_message;
};

} // namespace conditions

} // namespace sight::io::atoms::patch
