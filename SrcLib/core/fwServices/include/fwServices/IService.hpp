/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_ISERVICE_HPP__
#define __FWSERVICES_ISERVICE_HPP__

#include "fwServices/config.hpp"
#include "fwServices/factory/new.hpp"
#include "fwServices/helper/SigSlotConnection.hpp"

#include <fwCom/HasSignals.hpp>
#include <fwCom/HasSlots.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwCore/base.hpp>

#include <fwData/Object.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwThread/Worker.hpp>

#include <fwTools/Failed.hpp>
#include <fwTools/fwID.hpp>
#include <fwTools/macros.hpp>
#include <fwTools/Object.hpp>

#include <boost/property_tree/ptree.hpp>

#include <deque>

namespace fwServices
{
namespace registry
{
class ObjectService;
}

typedef std::pair< std::string, std::string > ObjectServiceKeyType;

/**
 * @brief   Base class for all services.
 * @class   IService
 *
 * @date    2007-2009.
 *
 * This class defines the API to use and declare services. The service state aims at imposing method execution order (i.e. configure(), start(), update() or update(const fwServices::ObjectMsg::sptr), stop()).
 *
 * @todo replace tests on status in start, stop, ... methods by assertions.
 * @todo Refactoring of SWAPPING status. Perhaps must be a special status as UPDATING or UPDATING must be another GlobalStatus. it must be homogeneous.
 * @todo Add a new method to test if m_associatedObject has expired
 */
class FWSERVICES_CLASS_API IService : public ::fwTools::Object,
                                      public ::fwCom::HasSlots,
                                      public ::fwCom::HasSignals
{

// to give to OSR an access on IService.m_associatedObject;
friend class registry::ObjectService;

public:
    typedef ::boost::property_tree::ptree ConfigType;

    fwCoreServiceClassDefinitionsMacro ( (IService)(::fwTools::Object) );
    fwCoreAllowSharedFromThis();

    /**
     * @name Definition of service status
     */

    //@{

    /// Defines all possible global status for a service, including transitions
    typedef enum
    {
        STARTED,    /**< state after start */
        STARTING,   /**< state during start */
        SWAPPING,   /**< state during swap */
        STOPPED,    /**< state after stop */
        STOPPING    /**< state during stop */
    } GlobalStatus;

    /// Defines all possible status for an update process
    typedef enum
    {
        UPDATING,   /**< state during update */
        NOTUPDATING /**< state during when the service is not updating */
    } UpdatingStatus;

    /// Defines all possible status for a configuration process
    typedef enum
    {
        CONFIGURING,    /**< state during configuration */
        CONFIGURED,     /**< state after configuration */
        UNCONFIGURED    /**< state when the service is not configured */
    } ConfigurationStatus;

    //@}


    /**
     * @name Slot API
     */
    //@{
    typedef ::boost::shared_future< void > SharedFutureType;
    typedef ::boost::packaged_task< void > PackagedTaskType;
    typedef ::boost::future< void > UniqueFutureType;

    FWSERVICES_API static const ::fwCom::Slots::SlotKeyType s_START_SLOT;
    typedef ::fwCom::Slot<SharedFutureType()> StartSlotType;

    FWSERVICES_API static const ::fwCom::Slots::SlotKeyType s_STOP_SLOT;
    typedef ::fwCom::Slot<SharedFutureType()> StopSlotType;

    FWSERVICES_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLOT;
    typedef ::fwCom::Slot<SharedFutureType()> UpdateSlotType;

    FWSERVICES_API static const ::fwCom::Slots::SlotKeyType s_SWAP_SLOT;
    typedef ::fwCom::Slot<SharedFutureType(::fwData::Object::sptr)> SwapSlotType;

    /// Initializes m_associatedWorker and associates this worker to all service slots
    FWSERVICES_API void setWorker( ::fwThread::Worker::sptr worker );

    /// Returns associate worker
    FWSERVICES_API ::fwThread::Worker::sptr getWorker() const;

    //@}

    /**
     * @name Key service API
     */

    //@{

    /**
     * @brief Affect the configuration, using a generic XML like structure.
     * @param[in] _cfgElement a structure which represents the xml configuration
     * @post m_configurationState == UNCONFIGURED
     */
    FWSERVICES_API void setConfiguration( const ::fwRuntime::ConfigurationElement::sptr _cfgElement );

    /**
     * @brief Affect the configuration, using a boost property tree
     * @param[in] ptree property tree
     * @post m_configurationState == UNCONFIGURED
     */
    FWSERVICES_API void setConfiguration( const ConfigType &ptree );

    /**
     * @brief Invoke configuring() if m_globalState == STOPPED. Invoke reconfiguring() if m_globalState == STARTED. Does nothing otherwise.
     * @pre m_configurationState == UNCONFIGURED
     * @post m_configurationState == CONFIGURED
     * @note invoke checkConfiguration()
     */
    FWSERVICES_API void configure();

    /**
     * @brief Invoke starting() if m_globalState == STOPPED. Does nothing otherwise.
     * @post m_globalState == STARTED
     */
    FWSERVICES_API SharedFutureType start(); //throw( ::fwTools::Failed );

    /**
     * @brief Invoke stopping() if m_globalState == STARTED. Does nothing otherwise. Stops all observations.
     *
     * @post m_globalState == STOPPED
     *
     */
    FWSERVICES_API SharedFutureType stop(); //throw( ::fwTools::Failed );

    /**
     * @brief Invoke updating() if m_globalState == STARTED. Does nothing otherwise.
     * @pre m_globalState == STARTED
     */
    FWSERVICES_API SharedFutureType update(); //throw( ::fwTools::Failed );

    /**
     * @brief Associate the service to another object
     * @param[in] _obj change association service from m_associatedObject to _obj
     * @pre m_globalState == STARTED
     * @pre m_associatedObject != _obj
     *
     * This method provides to associate te service to another object without stopping
     * and deleting it. Furthermore, this method modify all observations to be aware to
     * _obj notifications.
     *
     *
     */
    FWSERVICES_API SharedFutureType swap( ::fwData::Object::sptr _obj ); //throw( ::fwTools::Failed );

    //@}

    /**
     * @name All concerning status access
     */

    //@{

    /**
     * @brief Return the global process status
     * @return m_globalState
     */
    FWSERVICES_API GlobalStatus getStatus() const throw();

    /**
     * @brief Test if the service is started or not
     * @return true if m_globalState == STARTED
     */
    FWSERVICES_API bool isStarted() const throw();

    /**
     * @brief Test if the service is stopped or not
     * @return true if m_globalState == STOPPED
     */
    FWSERVICES_API bool isStopped() const throw();

    /**
     * @brief Return the configuration process status
     * @return m_configurationState
     */
    FWSERVICES_API ConfigurationStatus getConfigurationStatus() const throw();

    /**
     * @brief Return the update process status
     * @return m_updatingState
     */
    FWSERVICES_API UpdatingStatus getUpdatingStatus() const throw();
    //@}


    /**
     * @name All concerning configuration
     */

    //@{

    /**
     * @brief Return the configuration, in an xml format read using runtime library
     * @return m_configuration, a structure which represents the service configuration
     */
    FWSERVICES_API ::fwRuntime::ConfigurationElement::sptr getConfiguration() const;

    /**
     * @brief Return the configuration, in an boost property tree
     */
    FWSERVICES_API ConfigType getConfigTree() const;


//    /**
//     * @brief Check the configuration using XSD if possible
//     * @return true if the configuration is validate or if
//     * there is not XSD available to validate the configuration.
//     */
//    FWSERVICES_API bool checkConfiguration() ;


    //@}


    /**
     * @name Optimized access to associated Object & Helper
     */
    //@{

    /**
     * @brief Return the object associated to service
     * @return m_associatedObject
     * @pre the service must have an associated object set
     * @pre associated object does not be expired
     */
    FWSERVICES_API ::fwData::Object::sptr getObject();

    /**
     * @brief Return the object associated to service. The object is casted.
     * @return m_associatedObject casted in a good type
     * @pre the service must have an associated object set
     * @pre associated object does not be expired
     * @post cast verification in debug mode ( assertion on dynamic cast )
     */
    template< class DATATYPE > SPTR(DATATYPE) getObject();

    //@}

    /**
     * @name Communication connection between object::signals and service::slots
     */
    //@{

    typedef ::fwServices::helper::SigSlotConnection::KeyConnectionsType KeyConnectionsType;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     */
    FWSERVICES_API virtual KeyConnectionsType getObjSrvConnections() const;

    //@}

    /**
     * @name Misc
     */

    //@{

    /**
     * @brief Streaming a service
     * @see IService::operator<<(std::ostream & _ostream, IService& _service)
     * @note Invoke IService::info( std::ostream )
     */
    FWSERVICES_API friend std::ostream & operator<<(std::ostream & _sstream, IService & _service);

    //@}



protected:

    /**
     * @name Constructor and Destructor
     */

    //@{

    /**
     * @brief IService constructor.
     *
     * This constructor does nothing. By default, m_associatedObject is null and
     * service is considered as STOPPED, NOTUPDATING and UNCONFIGURED.
     */
    FWSERVICES_API IService();

    /**
     * @brief IService desctructor.
     *
     * This destructor does nothing.
     */
    FWSERVICES_API virtual ~IService();

    //@}

    /**
     * @name Interface to override (implementation contract)
     */

    //@{

    /**
     * @brief Initialize the service activity.
     *
     * Use configuration to start his job. For example : the start method
     * installs a button in a frame and show the frame.
     * @see start()
     */
    FWSERVICES_API virtual void starting() throw ( ::fwTools::Failed ) = 0;

    /**
     * @brief Uninitialize the service activity. The stop() method is always invoked before destroying a service.
     *
     * @see stop()
     */
    FWSERVICES_API virtual void stopping() throw ( ::fwTools::Failed ) = 0;

    /**
     * @brief Swap the service from associated object to another object
     * @see swap()
     * @todo This method must be pure virtual
     * @todo FIXME after code update for all services
     * @todo This method must have in parameter the new object or the old ?
     */
    virtual void swapping() throw ( ::fwTools::Failed )
    {
    }

    /**
     * @brief Configure the service before starting. Apply the configuration to service.
     * @see configure()
     */
    FWSERVICES_API virtual void configuring() throw ( ::fwTools::Failed ) = 0;

    /**
     * @brief Reconfigure the service activity when is started.
     * @todo This method should be pure virtual
     * @see configure()
     */
    FWSERVICES_API virtual void reconfiguring() throw ( ::fwTools::Failed );

    /**
     * @brief Perform some computations according to object (this service is attached to) attribute values and its internal state.
     * @see update()
     */
    FWSERVICES_API virtual void updating() throw ( ::fwTools::Failed ) = 0;

    /**
     * @brief Write information in a stream.
     *
     * This method is used by operator<<(std::ostream & _sstream, IService& _service)
     * to avoid declaration of << by all services.
     */
    FWSERVICES_API virtual void info( std::ostream & _sstream );

    //@}

    /**
     * @brief Configuration element used to configure service internal state using a generic XML like structure
     */
    ::fwRuntime::ConfigurationElement::sptr m_configuration;

    /**
     * @brief associated object of service
     * @todo this field must be private
     */
    ::fwData::Object::wptr m_associatedObject;

    /**
     * @name Slot API
     */
    //@{

    /// Slot to call start method
    StartSlotType::sptr m_slotStart;

    /// Slot to call stop method
    StopSlotType::sptr m_slotStop;

    /// Slot to call update method
    UpdateSlotType::sptr m_slotUpdate;

    /// Slot to call swap method
    SwapSlotType::sptr m_slotSwap;

    /// Associated worker
    ::fwThread::Worker::sptr m_associatedWorker;

    //@}

private:

    /**
     * @brief Defines the current global status of the service.
     */
    GlobalStatus m_globalState;

    /**
     * @brief Defines if the service is updating.
     */
    UpdatingStatus m_updatingState;

    /**
     * @brief Defines if the service is configured or not.
     */
    ConfigurationStatus m_configurationState;

};

} // namespace fwServices

#include "fwServices/IService.hxx"

#endif // __FWSERVICES_ISERVICE_HPP__
