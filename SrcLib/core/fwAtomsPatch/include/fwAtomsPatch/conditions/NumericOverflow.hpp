/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMSPATCH_CONDITIONS_NUMERICOVERFLOW_HPP__
#define __FWATOMSPATCH_CONDITIONS_NUMERICOVERFLOW_HPP__

#include <fwAtoms/Numeric.hpp>

#include "fwAtomsPatch/conditions/Abstract.hpp"
#include "fwAtomsPatch/config.hpp"

namespace fwAtomsPatch
{
namespace conditions
{
/**
 * @class NumericOverflow
 * @brief Applies condition on numeric value.
 */
class FWATOMSPATCH_CLASS_API NumericOverflow : public Abstract
{
public:
    fwCoreClassDefinitionsWithFactoryMacro((NumericOverflow), (()), new NumericOverflow);
    fwCoreAllowSharedFromThis();

    /// Numerical test
    FWATOMSPATCH_API virtual bool test(::fwAtoms::Base::sptr old, ::fwAtoms::Base::sptr newValue);

    /// Numerical test
    FWATOMSPATCH_API virtual bool test(::fwAtoms::Base::sptr old);

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

#endif /* __FWATOMSPATCH_CONDITIONS_NUMERICOVERFLOW_HPP__ */
