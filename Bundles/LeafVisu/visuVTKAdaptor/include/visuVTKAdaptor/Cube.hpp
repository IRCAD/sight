/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_CUBE_HPP__
#define __VISUVTKADAPTOR_CUBE_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwRenderVTK/IAdaptor.hpp>

namespace visuVTKAdaptor
{

/**
 * @brief Render a cube on the generic scene
 */
class VISUVTKADAPTOR_CLASS_API Cube : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (Cube)(::fwRenderVTK::IAdaptor) );

protected:
    VISUVTKADAPTOR_API void doStart();
    VISUVTKADAPTOR_API void doStop();

    //------------------------------------------------------------------------------

    void doSwap()
    {
    }
    //------------------------------------------------------------------------------

    void doUpdate()
    {
    }
    //------------------------------------------------------------------------------

    void doConfigure()
    {
    }
};

#endif //__VISUVTKADAPTOR_CUBE_HPP__

} //namespace visuVTKAdaptor
