/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _CTRLSELECTION_MANAGER_SWAPPER_HPP_
#define _CTRLSELECTION_MANAGER_SWAPPER_HPP_

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwServices/IService.hpp>
#include "ctrlSelection/config.hpp"
#include "ctrlSelection/IManagerSrv.hpp"

namespace ctrlSelection
{

/**
 * @namespace ctrlSelection::manager
 * @brief     Namespace contained the manager services
 */
namespace manager
{

/**
 * @class  SwapperSrv.
 * @brief  This services is a manager which starts, stops or swaps services on object contained in a composite when
 * it receive specific message (mainly sent by updater).
 * @author IRCAD (Research and Development Team).

 * @date   2007-2009.
 */
class CTRLSELECTION_CLASS_API SwapperSrv : public ::ctrlSelection::IManagerSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (SwapperSrv)(::ctrlSelection::IManagerSrv) ) ;

    /// Constructor.  Do nothing.
    CTRLSELECTION_API SwapperSrv() throw() ;

    /// Destructor. Do nothing.
    CTRLSELECTION_API virtual ~SwapperSrv() throw() ;

protected:

    /// Implements starting method derived from IService. Starts the managed services if their objects are in the composite.
    /// If the mode is "dummy", starts the the managed services on dummy objects if their objects are not in the composite.
    CTRLSELECTION_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Implements stopping method derived from IService. Stops and erases all the managed services.
    CTRLSELECTION_API virtual void stopping()  throw ( ::fwTools::Failed );

    /**
    * @brief Implements configuring method derived from IService. .
    *
    * Sample of declaration configuration for a simple swapper service
    *
    * @verbatim
        <service uid="myManager" implementation="::ctrlSelection::manager::SwapperSrv" type="::ctrlSelection::IManagerSrv" autoComChannel="yes" >
            <mode type="dummy" />
            <config>
                <object id="myImage" type="::fwData::Image" >
                    <service uid="myMedicalImageConverter" implementation="::ctrlSelection::MedicalImageSrv" type="::fwServices::IController"  autoComChannel="no" />
                    <service uid="myServices" implementation="..." type="..." autoComChannel="yes" />
                </object>
                <object id="myAcquisition" type="::fwData::Acquisition" >
                    <service uid="myServices2" implementation="..." type="..." autoComChannel="yes" />
                </object>
            </config>
        </service>
    @endverbatim
    * With:
    * @li mode : must be "stop" or "dummy". The dummy mode doesn't stop the services when its attached object is deleted but swap it on a dummy object.
    * @li the objects and services tags are defined as same as the configuration of objects and services.
    */
    CTRLSELECTION_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// Implements reconfiguring method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void reconfiguring()  throw ( ::fwTools::Failed );

    /// Implements updating method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void updating() throw ( ::fwTools::Failed );

    /// Implements info method derived from IService. Print classname.
    CTRLSELECTION_API virtual void info( std::ostream &_sstream );

    /// Reacts on specifics event (ADDED_FIELDS, REMOVED_FIELDS and SWAPPED_FIELDS) and start, stop or swap the managed services
    /// on the objects defined in the message or dummy objects
    CTRLSELECTION_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );


    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    typedef std::string ObjectIdType;


    class SubService
    {
    public:
        SubService()
        { }
        ~SubService()
        { }
        SPTR (::fwServices::IService) getService()
                    { return m_service.lock(); }

        ::fwTools::Object::sptr m_dummy;
        ConfigurationType m_config;
        WPTR(::fwServices::IService) m_service;
    };

    typedef std::vector< SPTR(SubService) > SubServicesVecType;
    typedef std::map< ObjectIdType, SubServicesVecType > SubServicesMapType ;


    void initOnDummyObject( ConfigurationType conf );
    void addObjects( ::fwData::Composite::sptr _composite );
    void addObject( const std::string objectId, ::fwTools::Object::sptr object );
    void swapObjects( ::fwData::Composite::sptr _composite );
    void swapObject(const std::string objectId, ::fwTools::Object::sptr object);
    void removeObjects( ::fwData::Composite::sptr _composite );
    void removeObject( const std::string objectId );

private:
    bool m_dummyStopMode;
    ConfigurationType m_managerConfiguration;
    SubServicesMapType m_objectsSubServices;

};

} // manager
} // ctrlSelection

#endif // _CTRLSELECTION_MANAGER_SWAPPER_HPP_
