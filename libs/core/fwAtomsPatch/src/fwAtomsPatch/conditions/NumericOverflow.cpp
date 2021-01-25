/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "fwAtomsPatch/conditions/NumericOverflow.hpp"

#include "fwAtomsPatch/infos/log.hpp"

#include <atoms/Numeric.hpp>
#include <atoms/Numeric.hxx>

namespace fwAtomsPatch
{
namespace conditions
{

NumericOverflow::NumericOverflow(atoms::Base::sptr defaultValue) :
    Abstract(defaultValue)
{
}

// ----------------------------------------------------------------------------

void NumericOverflow::setTreshold(atoms::Numeric::sptr treshold)
{
    m_treshold = treshold;
}

// ----------------------------------------------------------------------------

bool NumericOverflow::test(atoms::Base::sptr old,
                           atoms::Base::sptr newValue)
{
    return this->test(newValue);
}

// ----------------------------------------------------------------------------

bool NumericOverflow::test(atoms::Base::sptr value)
{
    if(value->isNumeric())
    {
        atoms::Numeric::sptr numeric = atoms::Numeric::dynamicCast(value);
        return numeric->getValue<double>() <= m_treshold->getValue<double>();
    }
    else
    {
        fwAtomsPatchErrorLogMacro("the value is not a numeric");
        return false;
    }
}

// ----------------------------------------------------------------------------

NumericOverflow::sptr NumericOverflow::New(atoms::Base::sptr defaultValue)
{
    NumericOverflow::sptr valueSptr(new NumericOverflow(defaultValue));
    return valueSptr;
}

} //conditions
} //fwAtomsPatch
