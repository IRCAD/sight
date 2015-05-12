/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
