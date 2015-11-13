/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_AXES_HPP__
#define __VISUVTKADAPTOR_AXES_HPP__

#ifndef ANDROID

// Config include
#include "visuVTKAdaptor/config.hpp"

// FW4SPL Includes
#include <fwRenderVTK/IVtkAdaptorService.hpp>

// Boost Includes
#include <boost/logic/tribool.hpp>

// VTK Includes
#include <vtkAxesActor.h>

namespace visuVTKAdaptor
{

/**
 * @brief Render axes in the generic scene.
 */
class VISUVTKADAPTOR_CLASS_API Axes : public ::fwRenderVTK::IVtkAdaptorService
{
public:

    fwCoreServiceClassDefinitionsMacro ( (Axes)(::fwRenderVTK::IVtkAdaptorService) );

    VISUVTKADAPTOR_API Axes() throw();
    VISUVTKADAPTOR_API virtual ~Axes() throw();

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);

    void buildPipeline();

    vtkAxesActor * m_axesActor;
    double m_length;
    bool m_labelOn;
};

} // namespace visuVTKAdaptor

#endif // ANDROID

#endif // __VISUVTKADAPTOR_AXES_HPP__
