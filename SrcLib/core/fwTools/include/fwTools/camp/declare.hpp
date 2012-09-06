/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_CAMP_DECLARE_HPP__
#define __FWDATA_CAMP_DECLARE_HPP__

#include <fwCamp/macros.hpp>
#include "fwTools/BufferObject.hpp"
#include "fwTools/Object.hpp"

namespace fwTools
{

namespace camp {

struct runner
{
    runner()
    {
        localDeclarefwToolsBufferObject();
        // localDeclarefwToolsObject();
    }

    static runner r;
};
}


}



#endif
