/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMSPATCH_CONDITION_ABSTRACT_HPP__
#define __FWATOMSPATCH_CONDITION_ABSTRACT_HPP__

#include <fwCore/base.hpp>

#include <fwAtoms/Base.hpp>

#include "fwAtomsPatch/config.hpp"

namespace fwAtomsPatch
{
namespace conditions
{

/**
 * @class Abstract
 * @brief Defines a condition in the patch.
 */
class FWATOMSPATCH_CLASS_API Abstract : public ::fwCore::BaseObject
{
public:
    fwCoreClassDefinitionsWithFactoryMacro((Abstract), (()), new Abstract );
    fwCoreAllowSharedFromThis();

    /// Default value getter
    FWATOMSPATCH_API virtual ::fwAtoms::Base::sptr getDefaultValue();

    /// Test in replacement value
    FWATOMSPATCH_API virtual bool test(::fwAtoms::Base::sptr old, ::fwAtoms::Base::sptr newValue);

    /// Test with one value (remove/add/rename)
    FWATOMSPATCH_API virtual bool test(::fwAtoms::Base::sptr old);

protected:
    Abstract() {};
    Abstract(::fwAtoms::Base::sptr defaultValue);

    ::fwAtoms::Base::sptr m_defaultValue;
    std::string m_message;

};

} // namespace conditions

} // namespace fwAtomsPatch

#endif /* __FWATOMSPATCH_CONDITION_ABSTRACT_HPP__ */
