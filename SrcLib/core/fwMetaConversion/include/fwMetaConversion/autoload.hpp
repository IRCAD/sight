/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMETACONVERSION_AUTOLOAD_HPP__
#define __FWMETACONVERSION_AUTOLOAD_HPP__

#include "fwMetaConversion/custom/Factory.hpp"
#include "fwMetaConversion/custom/Graph.hpp"

namespace fwMetaConversion
{

struct runner
{
    runner()
    {
        fwMetaConversionRegisterMacro(::fwData::Graph,
                                      ::fwMetaConversion::custom::Graph);
    }

    static runner r;
};


}  // namespace fwMetaData


#endif /* __FWMETACONVERSION_AUTOLOAD_HPP__ */
