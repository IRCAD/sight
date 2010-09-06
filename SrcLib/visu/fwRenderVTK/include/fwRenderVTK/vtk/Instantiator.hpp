/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwRenderVTK/config.hpp"

namespace fwRenderVTK {

namespace vtk {

class FWRENDERVTK_CLASS_API Instantiator
{
public:
    FWRENDERVTK_API static void ClassInitialize();
    FWRENDERVTK_API static void ClassFinalize();
};
} //vtk

} //fwRenderVTK
