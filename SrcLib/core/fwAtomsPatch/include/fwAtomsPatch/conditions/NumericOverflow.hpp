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

#include "fwAtomsPatch/conditions/Abstract.hpp"
#include "fwAtomsPatch/config.hpp"

#include <fwAtoms/Numeric.hpp>

namespace fwAtomsPatch
{
namespace conditions
{
/**
 * @brief Applies condition on numeric value.
 */
class FWATOMSPATCH_CLASS_API NumericOverflow : public Abstract
{
public:
    fwCoreClassDefinitionsWithFactoryMacro((NumericOverflow), new NumericOverflow)
    fwCoreAllowSharedFromThis()

    /// Numerical test
    FWATOMSPATCH_API virtual bool test(::fwAtoms::Base::sptr old, ::fwAtoms::Base::sptr newValue) override;

    /// Numerical test
    FWATOMSPATCH_API virtual bool test(::fwAtoms::Base::sptr old) override;

    /// Set the maximum treshold
    FWATOMSPATCH_API virtual void setTreshold(::fwAtoms::Numeric::sptr treshold);

    /// Constructor
    FWATOMSPATCH_API static NumericOverflow::sptr New(::fwAtoms::Base::sptr defaultValue);

protected:
    NumericOverflow()
    {
    }
    NumericOverflow(::fwAtoms::Base::sptr defaultValue);

    ::fwAtoms::Numeric::sptr m_treshold;
};

} // namesapce conditions
} // namesapce fwAtomsPatch
