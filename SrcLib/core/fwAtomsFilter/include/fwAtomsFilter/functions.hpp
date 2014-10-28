/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMSFILTER_FUNCTIONS_HPP__
#define __FWATOMSFILTER_FUNCTIONS_HPP__

#include <fwCore/macros.hpp>

#include "fwAtomsFilter/config.hpp"

namespace fwAtoms
{
    class Object;
}
namespace fwAtomsFilter
{

    /**
     * @brief Returns true if given atom matches a defined ::fwMedData::Series implementations.
     * */
    FWATOMSFILTER_API bool isSeriesKnown(const SPTR(::fwAtoms::Object)& series);

} // namespace fwAtomsFilter

#endif // __FWATOMSFILTER_FUNCTIONS_HPP__

