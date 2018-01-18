/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
    fwCoreClassDefinitionsWithFactoryMacro((NumericOverflow), (()), new NumericOverflow)
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
