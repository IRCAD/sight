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

#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Numeric.hxx>

#include "fwAtomsPatch/infos/log.hpp"
#include "fwAtomsPatch/conditions/NumericOverflow.hpp"


namespace fwAtomsPatch
{
namespace conditions
{

NumericOverflow::NumericOverflow(::fwAtoms::Base::sptr defaultValue) : Abstract(defaultValue)
{
}

// ----------------------------------------------------------------------------

void NumericOverflow::setTreshold(::fwAtoms::Numeric::sptr treshold)
{
    m_treshold = treshold;
}

// ----------------------------------------------------------------------------

bool NumericOverflow::test(::fwAtoms::Base::sptr old,
                           ::fwAtoms::Base::sptr newValue)
{
    return this->test(newValue);
}

// ----------------------------------------------------------------------------

bool NumericOverflow::test(::fwAtoms::Base::sptr value)
{
    if(value->isNumeric())
    {
        ::fwAtoms::Numeric::sptr numeric = ::fwAtoms::Numeric::dynamicCast(value);
        return numeric->getValue<double>() <= m_treshold->getValue<double>();
    }
    else
    {
        fwAtomsPatchErrorLogMacro("the value is not a numeric");
        return false;
    }
}

// ----------------------------------------------------------------------------


NumericOverflow::sptr NumericOverflow::New(::fwAtoms::Base::sptr defaultValue)
{
    NumericOverflow::sptr valueSptr(new NumericOverflow(defaultValue));
    return valueSptr;
}

} //conditions
} //fwAtomsPatch
