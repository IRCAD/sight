/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_IADAPTOR_HPP__
#define __FWRENDEROGRE_IADAPTOR_HPP__

#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/IHasAdaptors.hpp"
#include "fwRenderOgre/SRender.hpp"

#include <fwCom/helper/SigSlotConnection.hpp>

#include <fwServices/IService.hpp>

#include <OGRE/OgreSceneManager.h>

#include <string>

namespace fwRenderOgre
{

/**
 * @brief Interface providing behavior of Ogre adaptor services
 */
class FWRENDEROGRE_CLASS_API IAdaptor : public ::fwServices::IService,
                                        public ::fwRenderOgre::IHasAdaptors
{
friend class SRender;
public:
    fwCoreNonInstanciableClassDefinitionsMacro( (IAdaptor)(::fwServices::IService) );

    /// Set the layer ID
    FWRENDEROGRE_API void setLayerID(const std::string& id);

    FWRENDEROGRE_API const std::string& getLayerID() const;

    /// Set the render service using this adaptor
    FWRENDEROGRE_API void setRenderService( SRender::sptr service );

    /// Get the render service using this adaptor
    FWRENDEROGRE_API SRender::sptr getRenderService() const;

    FWRENDEROGRE_API Layer::sptr getLayer() const;

    FWRENDEROGRE_API void connect();
    FWRENDEROGRE_API void disconnect();

protected:

    /// Constructor
    FWRENDEROGRE_API IAdaptor() noexcept;

    /// Destructor
    FWRENDEROGRE_API virtual ~IAdaptor() noexcept;

    //@{
    /// Overrides
    FWRENDEROGRE_API virtual void info(std::ostream& _sstream );
    //@}

    /**
     * @brief Parse common adaptor parameters:
     * @code{.xml}
       <config layer="..." />
       @endcode
     * @subsection Configuration Configuration:
     * - \b layer (mandatory): id of the layer where this adaptor applies.
     */
    FWRENDEROGRE_API void configureParams();

    /// Register the adaptor into its SRender service
    FWRENDEROGRE_API void initialize();

    /**
     * @brief Get the Ogre SceneManager
     * @return Ogre default scene manager
     */
    FWRENDEROGRE_API ::Ogre::SceneManager* getSceneManager();

    /// Ask the render service (SRender) to update
    FWRENDEROGRE_API virtual void requestRender();

    std::string msgHead() const;

    /// Layer ID
    ::std::string m_layerID;

    /// Render service which this adaptor is attached
    ::fwRenderOgre::SRender::wptr m_renderService;

    /// Signal/Slot connections with the objects
    ::fwCom::helper::SigSlotConnection m_objConnection;
};

// ------------------------------------------------------------------------

inline std::string IAdaptor::msgHead() const
{
    return "[ Adaptor '" + this->getID() + "'] ";
}
//------------------------------------------------------------------------------

} //namespace fwRenderOgre

#endif /*__FWRENDEROGRE_IADAPTOR_HPP__*/
