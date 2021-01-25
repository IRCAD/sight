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

#include "fwAtomsPatch/config.hpp"

#include <atoms/Base.hpp>

#include <core/base.hpp>

namespace fwAtomsPatch
{
namespace conditions
{

/**
 * @brief Defines a condition in the patch.
 */
class FWATOMSPATCH_CLASS_API Abstract : public core::BaseObject
{
public:
    fwCoreClassMacro(Abstract, core::BaseObject, new Abstract)
    fwCoreAllowSharedFromThis();

    /// Default value getter
    FWATOMSPATCH_API virtual atoms::Base::sptr getDefaultValue();

    /// Test in replacement value
    FWATOMSPATCH_API virtual bool test(atoms::Base::sptr old, atoms::Base::sptr newValue);

    /// Test with one value (remove/add/rename)
    FWATOMSPATCH_API virtual bool test(atoms::Base::sptr old);

protected:
    Abstract()
    {
    }
    Abstract(atoms::Base::sptr defaultValue);

    atoms::Base::sptr m_defaultValue;
    std::string m_message;

};

} // namespace conditions

} // namespace fwAtomsPatch
