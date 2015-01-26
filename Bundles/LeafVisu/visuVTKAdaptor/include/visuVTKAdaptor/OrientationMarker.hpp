/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_ORIENTATIONMARKER_HPP__
#define __VISUVTKADAPTOR_ORIENTATIONMARKER_HPP__

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"

namespace visuVTKAdaptor
{

/**
* @brief Adaptor using for displayed an orientationMarker in a generic scene
*/
class VISUVTKADAPTOR_CLASS_API OrientationMarker: public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (OrientationMarker)(::fwRenderVTK::IVtkAdaptorService) ) ;
    VISUVTKADAPTOR_API OrientationMarker();

protected:
    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    /**
     * @name Does nothing
     * @{ */
    void doSwap() throw(fwTools::Failed){};
    void doUpdate() throw(fwTools::Failed){};
    void doReceive(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed){};
    /**  @} */

   /**
     * @brief Configure the adaptor
     *
     * Example :
     @verbatim
     <adaptor id="visu" class="::visuVTKAdaptor::Text" objectId="self">
        <config renderer="default" hAlign="right" />
     </adaptor>
     @endverbatim
     */
    void configuring() throw(fwTools::Failed);

    /// Horizontal alignment
    std::string m_hAlign;
};
} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_ORIENTATIONMARKER_HPP__
