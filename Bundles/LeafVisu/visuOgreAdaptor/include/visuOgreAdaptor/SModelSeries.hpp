/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUOGREADAPTOR_SMODELSERIES_HPP__
#define __VISUOGREADAPTOR_SMODELSERIES_HPP__

#include "visuOgreAdaptor/config.hpp"

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/ITransformable.hpp>

namespace fwData
{
class Material;
}

namespace visuOgreAdaptor
{

/**
 * @brief This adaptor shows ModelSeries. Creates adaptors for each reconstruction in model.
 */
class VISUOGREADAPTOR_CLASS_API SModelSeries : public ::fwRenderOgre::IAdaptor,
                                               public ::fwRenderOgre::ITransformable
{

public:

    fwCoreServiceClassDefinitionsMacro( (SModelSeries)(::fwRenderOgre::IAdaptor) );

    /// Constructor.
    VISUOGREADAPTOR_API SModelSeries() throw();

    /// Destructor. Does nothing
    VISUOGREADAPTOR_API virtual ~SModelSeries() throw();

    /// Returns proposals to connect service slots to associated object signals
    ::fwServices::IService::KeyConnectionsType getObjSrvConnections() const;

protected:
    /// Creates a Transform Service, then updates.
    VISUOGREADAPTOR_API void doStart() throw(::fwTools::Failed);

    /**
     * @brief Configure the ModelSeries adaptor.
     * @code{.xml}
       <adaptor id="msAdaptorId" class="::visuOgreAdaptor::ModelSeries" objectId="objectId">
        <config transform="transform" texture="texture" autoresetcamera="autoresetcamera" />
       </adaptor>
       @endcode
     * With :
     *  - \b transform (mandatory) : the transformation matrix to associate to the adaptor
     *  - \b dynamic (optional) : if the modelSeries is likely to be updated frequently (performance hint)
     *  - \b texture : the OgreTexture to associate to the adaptor
     *  - \b autoresetcamera (optional)
     */
    VISUOGREADAPTOR_API void doConfigure() throw(::fwTools::Failed);
    /// Calls updating
    VISUOGREADAPTOR_API void doSwap() throw(::fwTools::Failed);
    /// Redraws all (stops then restarts sub services)
    VISUOGREADAPTOR_API void doUpdate() throw(::fwTools::Failed);
    /// Closes connections and unregisters service.
    VISUOGREADAPTOR_API void doStop() throw(::fwTools::Failed);

    /// Calls updating
    VISUOGREADAPTOR_API void addReconstruction();
    /// calls stopping.
    VISUOGREADAPTOR_API void removeReconstruction();

private:

    /// Defines if the camera must be reset automatically
    bool m_autoResetCamera;
    /// Texture adaptor's UID
    std::string m_textureAdaptorUID;
    /// Material name
    std::string m_materialTemplateName;
    /// Defines if the model series is dynamic
    bool m_isDynamic;
    /// Defines if the model series' vertices are dynamic
    bool m_isDynamicVertices;
    /// Signal/Slot connections with this service
    ::fwCom::helper::SigSlotConnection m_connections;
};

//------------------------------------------------------------------------------

} // visuOgreAdaptor

#endif // __VISUOGREADAPTOR_SMODELSERIES_HPP__
