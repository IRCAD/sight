/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWABSTRACTDATA_CAMP_DECLARE_HPP__
#define __FWABSTRACTDATA_CAMP_DECLARE_HPP__

#include "fwAtoms/Types.hpp"

#include "fwAtoms/config.hpp"

namespace fwAtoms {
namespace camp {

struct runner
{
    runner()
    {
        localDeclarefwAtomsBase();
        localDeclarefwAtomsString();
        localDeclarefwAtomsNumeric();
        localDeclarefwAtomsBoolean();
        localDeclarefwAtomsObject();
        localDeclarefwAtomsMap();
        localDeclarefwAtomsSequence();
        localDeclarefwAtomsBlob();
    }

    static runner r;
};


}  // namespace camp



}  // namespace fwAtoms


#endif /* __FWABSTRACTDATA_CAMP_DECLARE_HPP__ */
