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
#include "io/atoms/patch/conditions/Abstract.hpp"

#include <atoms/Numeric.hpp>

namespace sight::io::atoms::patch
{
namespace conditions
{
/**
 * @brief Applies condition on numeric value.
 */
class IO_ATOMS_CLASS_API NumericOverflow : public Abstract
{
public:
    fwCoreClassMacro(NumericOverflow, Abstract, new NumericOverflow)
    fwCoreAllowSharedFromThis()

    /// Numerical test
    IO_ATOMS_API virtual bool test(sight::atoms::Base::sptr old, sight::atoms::Base::sptr newValue) override;

    /// Numerical test
    IO_ATOMS_API virtual bool test(sight::atoms::Base::sptr old) override;

    /// Set the maximum treshold
    IO_ATOMS_API virtual void setTreshold(sight::atoms::Numeric::sptr treshold);

    /// Constructor
    IO_ATOMS_API static NumericOverflow::sptr New(sight::atoms::Base::sptr defaultValue);

protected:
    NumericOverflow()
    {
    }
    NumericOverflow(sight::atoms::Base::sptr defaultValue);

    sight::atoms::Numeric::sptr m_treshold;
};

} // namesapce conditions
} // namesapce fwAtomsPatch
