/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMCONVERSION_CONVERT_HPP__
#define __FWATOMCONVERSION_CONVERT_HPP__

#include <fwData/Object.hpp>
#include <fwAtoms/Object.hpp>

#include "fwAtomConversion/config.hpp"

namespace fwAtomConversion
{

    /// ???
    FWATOMCONVERSION_API ::fwAtoms::Object::sptr convert( ::fwData::Object::sptr data );

    /// ???
    FWATOMCONVERSION_API ::fwData::Object::sptr convert( ::fwAtoms::Object::sptr atom );

} // end namespace fwAtomConversion

#endif // __FWATOMCONVERSION_CONVERT_HPP__
