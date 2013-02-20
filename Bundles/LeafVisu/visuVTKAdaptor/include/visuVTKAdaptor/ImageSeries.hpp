/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKADAPTOR_ACQUISITION_HPP_
#define _VISUVTKADAPTOR_ACQUISITION_HPP_

#include <vector>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"

namespace visuVTKAdaptor
{


/**
 * @brief This adaptor shows ImageSeries. Creates an adaptor for this image in the series.
 * @class ImageSeries
 */
class VISUVTKADAPTOR_CLASS_API ImageSeries : public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (ImageSeries)(::fwRenderVTK::IVtkAdaptorService) ) ;

    VISUVTKADAPTOR_API ImageSeries() throw();

    VISUVTKADAPTOR_API virtual ~ImageSeries() throw();

    void setClippingPlanes(::fwRenderVTK::VtkRenderService::VtkObjectIdType id){ m_clippingPlanes = id ; }

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    /// redraw all (stop then restart sub services)
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

   VISUVTKADAPTOR_API void doReceive(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed);

    ::fwRenderVTK::VtkRenderService::VtkObjectIdType m_clippingPlanes;

private:
    bool   m_autoResetCamera;
};




} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_ACQUISITION_HPP_
