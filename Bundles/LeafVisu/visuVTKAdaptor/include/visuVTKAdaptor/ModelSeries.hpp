/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_MODELSERIES_HPP__
#define __VISUVTKADAPTOR_MODELSERIES_HPP__

#include <vector>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"

namespace visuVTKAdaptor
{


/**
 * @brief This adaptor shows ModelSeries. Creates adaptors for each reconstruction in model.
 * @class ModelSeries
 */
class VISUVTKADAPTOR_CLASS_API ModelSeries : public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (ModelSeries)(::fwRenderVTK::IVtkAdaptorService) );

    /**
     * @name Signals API
     * @{
     */
    typedef ::fwCom::Signal< void ( SPTR(::fwData::Material) ) > TextureAppliedSignalType;
    VISUVTKADAPTOR_API static const ::fwCom::Signals::SignalKeyType s_TEXTURE_APPLIED_SIG;
    /** @} */

    VISUVTKADAPTOR_API ModelSeries() throw();

    VISUVTKADAPTOR_API virtual ~ModelSeries() throw();

    void setClippingPlanes(::fwRenderVTK::VtkRenderService::VtkObjectIdType id)
    {
        m_clippingPlanes = id;
    }

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    /// redraw all (stop then restart sub services)
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    /// manage SHOW/ADD/REMOVED_RECONSTRUCTIONS event
    VISUVTKADAPTOR_API void doReceive(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed);

    ::fwRenderVTK::VtkRenderService::VtkObjectIdType m_clippingPlanes;

private:
    bool m_autoResetCamera;
    std::string m_textureAdaptorUID;

    /// Signal to emit when a texture must be applied on a material.
    TextureAppliedSignalType::sptr m_sigTextureApplied;
};




} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_MODELSERIES_HPP__
