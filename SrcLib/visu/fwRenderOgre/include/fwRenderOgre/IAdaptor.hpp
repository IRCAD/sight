/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_IADAPTOR_HPP__
#define __FWRENDEROGRE_IADAPTOR_HPP__

#include <fwRenderOgre/SRender.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/helper/SigSlotConnection.hpp>

#include <OGRE/OgreSceneManager.h>

#include <string>

#include "fwRenderOgre/config.hpp"

namespace fwRenderOgre
{

/**
 * @class IAdaptor
 * @brief Interface providing behavior of Ogre adaptor services
 */
class FWRENDEROGRE_CLASS_API IAdaptor : public fwServices::IService
{
friend class SRender;
public:
    fwCoreServiceClassDefinitionsMacro ( (IAdaptor)(::fwServices::IService) );

    typedef std::vector < ::fwRenderOgre::IAdaptor::wptr > AdaptorVector;
    typedef fwServices::IService SuperClass;

    /// Set the layer ID
    FWRENDEROGRE_API void setLayerID(const std::string& id);

    /// Set the render service using this adaptor
    FWRENDEROGRE_API void setRenderService( SRender::sptr service );

    /// Get the render service using this adaptor
    FWRENDEROGRE_API SRender::sptr getRenderService() const;

    /// Returns True or False wether a given adaptor is registered or not
    FWRENDEROGRE_API bool isAdaptorRegistered(::fwTools::fwID::IDType _adaptorID) const;

    /**
     * @brief Get all subservices linked to this adaptor
     * @return The vector of linked services
     */
    AdaptorVector& getRegisteredAdaptors();

    /// Returns the priority of the adaptor - some adaptors may have to be started before other ones
    FWRENDEROGRE_API virtual int getStartPriority();

protected:

    /// Constructor
    FWRENDEROGRE_API IAdaptor() throw();

    /// Destructor
    FWRENDEROGRE_API virtual ~IAdaptor() throw();

    /**
     * @name Standard service methods
     */
    //@{
    /// Overrides
    FWRENDEROGRE_API virtual void info(std::ostream &_sstream );
    FWRENDEROGRE_API void starting() throw(fwTools::Failed);
    FWRENDEROGRE_API void stopping() throw(fwTools::Failed);
    FWRENDEROGRE_API void swapping() throw(fwTools::Failed);
    FWRENDEROGRE_API void updating() throw(fwTools::Failed);
    FWRENDEROGRE_API void configuring() throw(fwTools::Failed);
    //@}

    /**
     * @name    Standard adaptor methods
     */
    //@{
    /// Pure virtual methods
    FWRENDEROGRE_API virtual void doStart()                                 = 0;
    FWRENDEROGRE_API virtual void doStop()                                  = 0;
    FWRENDEROGRE_API virtual void doSwap()                                  = 0;
    FWRENDEROGRE_API virtual void doUpdate()                                = 0;
    FWRENDEROGRE_API virtual void doConfigure() throw ( ::fwTools::Failed ) = 0;
    //@}

    /**
     * @brief Get the Ogre SceneManager
     * @return Ogre default scene manager
     */
    FWRENDEROGRE_API ::Ogre::SceneManager* getSceneManager();

    /**
     * @brief Register a new service linked to this adaptor
     */
    FWRENDEROGRE_API void registerService( ::fwRenderOgre::IAdaptor::sptr service );

    /**
     * @brief Unregister all services linked to this adaptor
     * @param classname Classname of services to unregister
     */
    FWRENDEROGRE_API void unregisterServices(std::string classname = "");

    template< class DATATYPE >
    SPTR(DATATYPE) getSafeInOut(const std::string& key) const;

    /// Ask the render service (SRender) to update
    FWRENDEROGRE_API virtual void requestRender();

    /// Layer ID
    ::std::string m_layerID;

    /// Render service which this adaptor is attached
    ::fwRenderOgre::SRender::wptr m_renderService;

    /// Signal/Slot connections with this service
    ::fwServices::helper::SigSlotConnection::sptr m_connections;

    /// Sub adaptors linked to this adaptor
    AdaptorVector m_subAdaptors;
};

//------------------------------------------------------------------------------

inline IAdaptor::AdaptorVector &IAdaptor::getRegisteredAdaptors()
{
    return m_subAdaptors;
}

//------------------------------------------------------------------------------

template< class DATATYPE >
SPTR(DATATYPE) IAdaptor::getSafeInOut(const std::string& key) const
{
    if( ::fwServices::IService::isVersion2() )
    {
        return this->getRenderService()->getInOut<DATATYPE>(key);
    }
    else
    {
        return std::dynamic_pointer_cast<DATATYPE>( ::fwTools::fwID::getObject(key) );
    }
}

//------------------------------------------------------------------------------

} //namespace fwRenderOgre

#endif /*__FWRENDEROGRE_IADAPTOR_HPP__*/
