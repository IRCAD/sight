/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_CUBE_HPP__
#define __VISUVTKADAPTOR_CUBE_HPP__

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"

namespace visuVTKAdaptor
{


/**
 * @brief Render a cube on the generic scene
 */
class VISUVTKADAPTOR_CLASS_API Cube : public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (Cube)(::fwRenderVTK::IVtkAdaptorService) );

protected:
    VISUVTKADAPTOR_API void doStart();
    VISUVTKADAPTOR_API void doStop();

    void doSwap()
    {
    }
    void doUpdate()
    {
    }
    void doConfigure()
    {
    }
};

#endif //__VISUVTKADAPTOR_CUBE_HPP__

} //namespace visuVTKAdaptor
