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
    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    void doSwap() throw(fwTools::Failed)
    {
    }
    void doUpdate() throw(fwTools::Failed)
    {
    }
    void doConfigure() throw(fwTools::Failed)
    {
    }
};

#endif //__VISUVTKADAPTOR_CUBE_HPP__

} //namespace visuVTKAdaptor
