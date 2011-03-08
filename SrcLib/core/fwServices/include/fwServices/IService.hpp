/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_ISERVICE_HPP_
#define _FWSERVICES_ISERVICE_HPP_

#include <deque>

#include <fwTools/Failed.hpp>
#include <fwTools/Object.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwServices/config.hpp"
#include "fwServices/ObjectMsg.hpp"

namespace fwServices
{
class IEditionService;
namespace registry
{
class ObjectService;
}

typedef std::pair< std::string , std::string > ObjectServiceKeyType ;

/**
 * @brief   Base class for all services.
 * @class   IService.
 * @author IRCAD (Research and Development Team).
 * @date    2007-2009.
 *
 * This class defines the API to use and declare services. The service state aims at imposing method execution order (i.e. configure(), start(), update() or update(const fwServices::ObjectMsg::sptr), stop()).
 *
 * @todo replace tests on status in start, stop, ... methods by assertions.
 * @todo Refactoring of SWAPPING status. Perhaps must be a special status as UPDATING or UPDATING must be another GlobalStatus. it must be homogeneous.
 * @todo Add a new method to test if m_associatedObject has expired
 */
class FWSERVICES_CLASS_API IService : public ::fwTools::Object
{

    // to give to OSR an access on IService.m_associatedObject;
    friend class registry::ObjectService;
    friend class IEditionService;

public :

    fwCoreServiceClassDefinitionsMacro ( (IService)(::fwTools::Object) ) ;

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

    /// Defines all possible service status concerning communication
    typedef enum
    {
        SENDING_MSG,    /**< state during the service notifies to listeners a message */
        RECEIVING_WITH_SENDING_MSG, /**< state during the service notifies to listeners a message when it is already receiving a message */
        RECEIVING_MSG,  /**< state during the service receives a message and analyzed it*/
        IDLE            /**< state when service do nothing */
    } NotificationStatus;

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
     * @name Key service API
     */

    //@{

    /**
     * @brief Affect the configuration, using a generic XML like structure.
     * @param[in] _cfgElement a structure which represents the xml configuration
     * @post m_configurationState == UNCONFIGURED
     */
    FWSERVICES_API void setConfiguration( const ::fwRuntime::ConfigurationElement::sptr _cfgElement ) ;

    /**
     * @brief Invoke configuring() if m_globalState == STOPPED. Invoke reconfiguring() if m_globalState == STARTED. Does nothing otherwise.
     * @pre m_configurationState == UNCONFIGURED
     * @post m_configurationState == CONFIGURED
     * @note invoke checkConfiguration()
     */
    FWSERVICES_API void configure() ;

    /**
     * @brief Invoke starting() if m_globalState == STOPPED. Does nothing otherwise.
     * @post m_globalState == STARTED
     */
    FWSERVICES_API void start() throw( ::fwTools::Failed );

    /**
     * @brief Invoke stopping() if m_globalState == STARTED. Does nothing otherwise. Stops all observations (ICommunication for which this is destination).
     *
     * @post m_globalState == STOPPED
     *
     */
    FWSERVICES_API void stop() throw( ::fwTools::Failed );

    /**
     * @brief Invoke updating() if m_globalState == STARTED. Does nothing otherwise.
     * @pre m_globalState == STARTED
     */
    FWSERVICES_API void update() throw( ::fwTools::Failed );

    /**
     * @brief Invoke updating(fwServices::ObjectMsg::csptr) if m_globalState == STARTED. Does nothing otherwise. This method makes a service assimilable to an observer in the sense of the observer design pattern.
     * @pre m_globalState == STARTED
     * @pre m_notificationState == IDLE
     */
    FWSERVICES_API void update( fwServices::ObjectMsg::csptr _msg )  ;

    /**
     * @brief Associate the service to another object
     * @param[in] _obj change association service from m_associatedObject to _obj
     * @pre m_globalState == STARTED
     * @pre m_associatedObject != _obj
     *
     * This method provides to associate te service to another object without stopping
     * and deleting it. Furthermore, this method modify all observations to be aware to
     * _obj notifications.
     * @author IRCAD (Research and Development Team).
     * @author  IRCAD (Research and Development Team).
     */
    FWSERVICES_API void swap( ::fwTools::Object::sptr _obj ) throw( ::fwTools::Failed );

    //@}

    /**
     * @name Management of event handler configuration
     */

    //@{

    /**
     * @brief this method permits to add a new event handled by the service.
     * @param[in] a new id analyze by the service.
     * @post isHandlingAllEvents() == false
     */
    FWSERVICES_API void addNewHandledEvent( std::string _eventId );

    /**
     * @brief Get a vector of the handling events.
     * @param[out] the vector of events.
     * @pre isHandlingAllEvents() == false
     *
     * This method returns a vector of the events which are handle by the services, by default all events are handle if this method is not overwritted.
     */
    FWSERVICES_API std::vector< std::string > getHandledEvents();

    /// Return if this service analyse all events, if the value is equal to false, see the handling configuration with getHandledEvents() method.
    FWSERVICES_API bool isHandlingAllEvents();

    /**
     * @brief When any handling event is defined in service, to not listen notifications event if the observation is set on.
     * @pre m_handledEvents must be empty
     * @post m_isHandlingAllEvents == false
     */
    FWSERVICES_API void handlingEventOff();

    //@}


    /**
     * @name All concerning status access
     */

    //@{

    /**
     * @brief Return the global process status
     * @return m_globalState
     */
    FWSERVICES_API GlobalStatus getStatus() const throw() ;

    /**
     * @brief Test if the service is started or not
     * @return true if m_globalState == STARTED
     */
    FWSERVICES_API bool isStarted() const throw() ;

    /**
     * @brief Test if the service is stopped or not
     * @return true if m_globalState == STOPPED
     */
    FWSERVICES_API bool isStopped() const throw() ;

    /**
     * @brief Test if the service is sending message.
     * @return true if m_notificationState == SENDING or RECEIVING_WITH_SENDING_MSG
     */
    FWSERVICES_API bool isSending() const throw() ;

    /**
     * @brief Return the configuration process status
     * @return m_configurationState
     */
    FWSERVICES_API ConfigurationStatus getConfigurationStatus() const throw() ;

    /**
     * @brief Return the communication state of the service
     * @return m_notificationState
     */
    FWSERVICES_API NotificationStatus getNotificationStatus() const throw() ;

    /**
     * @brief Return the update process status
     * @return m_updatingState
     */
    FWSERVICES_API UpdatingStatus getUpdatingStatus() const throw() ;
    //@}


    /**
     * @name All concerning configuration
     */

    //@{

    /**
     * @brief Return the configuration, in an xml format read using runtime library
     * @return m_configuration, a structure which represents the service configuration
     * @todo getConfiguration() must not be virtual.
     * @todo getConfiguration() must be const.
     */
    FWSERVICES_API virtual ::fwRuntime::ConfigurationElement::sptr getConfiguration() ;

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
    FWSERVICES_API ::fwTools::Object::sptr getObject();

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
     * @name Misc
     */

    //@{

    /**
     * @brief Streaming a service
     * @see IService::operator<<(std::ostream & _ostream, IService& _service)
     * @note Invoke IService::info( std::ostream )
     */
    FWSERVICES_API friend std::ostream & operator<<(std::ostream & _sstream, IService & _service) ;

    //@}

protected :

    /**
     * @name Constructor and Destructor
     */

    //@{

    /**
     * @brief IService constructor.
     *
     * This constructor does nothing. By default, m_associatedObject is null and
     * service is considered as STOPPED, IDLE and UNCONFIGURED.
     */
    FWSERVICES_API IService() ;

    /**
     * @brief IService desctructor.
     *
     * This destructor does nothing.
     */
    FWSERVICES_API virtual ~IService() ;

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
    FWSERVICES_API virtual void starting() throw ( ::fwTools::Failed ) = 0 ;

    /**
     * @brief Uninitialize the service activity. The stop() method is always invoked before destroying a service.
     *
     * @see stop()
     */
    FWSERVICES_API virtual void stopping() throw ( ::fwTools::Failed ) = 0 ;

    /**
     * @brief Swap the service from associated object to another object
     * @see swap()
     * @todo This method must be pure virtual
     * @todo FIXME after code update for all services
     * @todo This method must have in parameter the new object or the old ?
     */
    FWSERVICES_API virtual void swapping() throw ( ::fwTools::Failed ) {} ;

    /**
     * @brief Configure the service before starting. Apply the configuration to service.
     * @see configure()
     */
    FWSERVICES_API virtual void configuring() throw ( ::fwTools::Failed ) = 0 ;

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
    FWSERVICES_API virtual void updating() throw ( ::fwTools::Failed ) = 0 ;

    /**
     * @brief Perform some computations according to modifications specified in the _msg parameter. _msg generally indicates modification to occur (or having occured) on the object the service
     * is attached to.
     * @see update(fwServices::ObjectMsg::csptr )
     */
    FWSERVICES_API virtual void updating( fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed ) = 0 ;

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
    ::fwRuntime::ConfigurationElement::sptr  m_configuration ;

    /**
     * @brief associated object of service
     * @todo this field must be private
     */
    ::fwTools::Object::wptr m_associatedObject;

private :

    /**
     * @brief Defines the current global status of the service.
     */
    GlobalStatus m_globalState;

    /**
     * @brief Defines if the service is receiving or/and sending message.
     */
    NotificationStatus m_notificationState;

    /**
     * @brief Defines if the service is updating.
     */
    UpdatingStatus m_updatingState;

    /**
     * @brief Defines if the service is configured or not.
     */
    ConfigurationStatus m_configurationState;

    std::deque< ::fwServices::ObjectMsg::csptr > m_msgDeque;

    /// Specific event configuration, by default this vector is empty and m_isHandlingAllEvents is set to true.
    std::vector< std::string > m_handledEvents;

    /// To know if service listens all events (default value) or if it has a specific event configuration.
    bool m_isHandlingAllEvents;


    /**
     * @brief Switch communication status as SENDING_MSG ( if current state is IDLE ) or RECEIVING_WITH_SENDING_MSG ( if current state is RECEIVING_MSG ).
     * @pre this->isSending() == false
     * @see isSending()
     * @post this->isSending() == true
     */
    void sendingModeOn();

    /**
     * @brief Switch communication status as IDLE ( if current state is SENDING_MSG ) or RECEIVING_MSG ( if current state is RECEIVING_WITH_SENDING_MSG ).
     * @pre this->isSending() == true
     * @see isSending()
     * @post this->isSending() == false
     */
    void sendingModeOff();

    /**
     * @brief process the message(s) which are in the waiting queue(m_msgDeque).
     * @pre
     * @post m_msgDeque will be empty.
     */
    void processingPendingMessages();

};

} // namespace fwServices

#include "fwServices/IService.hxx"

#endif // _FWSERVICES_ISERVICE_HPP_
