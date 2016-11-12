/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLSELECTION_MANAGER_SFIELD_HPP__
#define __CTRLSELECTION_MANAGER_SFIELD_HPP__

#include "ctrlSelection/IManagerSrv.hpp"
#include "ctrlSelection/config.hpp"

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/IService.hpp>

namespace ctrlSelection
{
namespace manager
{

/**
 * @brief  This services is a manager which starts, stops or swaps services on field contained in a composite when
 * it receive specific message (mainly sent by updater).
 */
class CTRLSELECTION_CLASS_API SField : public ::ctrlSelection::IManagerSrv
{

public:

    fwCoreServiceClassDefinitionsMacro( (SField)(::ctrlSelection::IManagerSrv) );

    /// Constructor.  Do nothing.
    CTRLSELECTION_API SField() throw();

    /// Destructor. Do nothing.
    CTRLSELECTION_API virtual ~SField() throw();

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Object::s_ADDED_FIELDS_SIG to this::s_ADD_FIELDS_SLOT
     * Connect Object::s_CHANGED_FIELDS_SIG to this::s_CHANGE_FIELDS_SLOT
     * Connect Object::s_REMOVED_FIELDS_SIG to this::s_REMOVE_FIELDS_SLOT
     */
    CTRLSELECTION_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:

    /// Implements starting method derived from IService. Starts the managed services if their fields are in the
    // composite.
    /// If the mode is "dummy", starts the the managed services on dummy fields if their fields are not in the
    // composite.
    CTRLSELECTION_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Implements stopping method derived from IService. Stops and erases all the managed services.
    CTRLSELECTION_API virtual void stopping()  throw ( ::fwTools::Failed );

    /**
     * @brief Implements configuring method derived from IService. .
     *
     * Sample of declaration configuration for a simple swapper service
     *
     * @code{.xml}
        <service uid="FieldManager" impl="::ctrlSelection::manager::SField" type="::ctrlSelection::IManagerSrv"
     * autoConnect="yes" >
            <mode type="dummy" />
            <config>
                <field id="TFSelection" type="::fwData::TransferFunction" >
                    <service uid="myServices" impl="..." type="..." autoConnect="yes" />
                    <connect>
                        <signal>key</signal>
                        <slot>uid/key</slot>
                    </connect>
                </field>
                <field id="AxialSliceIndex" type="::fwData::Integer" >
                    <service uid="Services2" impl="..." type="..." autoConnect="yes" worker="myThread" />
                    <proxy channel="...">
                        <signal>...</signal>
                        <signal>.../...</signal>
                        <slot>.../...</slot>
                        <slot>.../...</slot>
                    </proxy>
                </field>
            </config>
        </service>
       @endcode
     * With:
     * @li mode : must be "stop" or "dummy". The dummy mode doesn't stop the services when its attached field is deleted
     * but swap it on a dummy field.
     * @li the fields, services, connect and proxy tags are defined as same as the configuration of fields and services.
     * @li autoConnect: optional (default value = false), if true allows to listen signals from the associated object.
     * @li worker: optional, allows to manage the service in another thread.
     */
    CTRLSELECTION_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// Implements reconfiguring method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void reconfiguring()  throw ( ::fwTools::Failed );

    /// Implements updating method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void updating() throw ( ::fwTools::Failed );

    /// Implements info method derived from IService. Print classname.
    CTRLSELECTION_API virtual void info( std::ostream& _sstream );

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    typedef ::fwData::Object::FieldNameType FieldNameType;

    class SubService
    {
    public:

        SubService() :
            m_hasAutoConnection(false)
        {
        }

        ~SubService()
        {
        }

        SPTR(::fwServices::IService) getService()
        {
            return m_service.lock();
        }

        ::fwData::Object::sptr m_dummy;
        ConfigurationType m_config;
        WPTR(::fwServices::IService) m_service;
        ::fwCom::helper::SigSlotConnection m_connections;
        bool m_hasAutoConnection;
    };

    typedef std::vector< SPTR(SubService) > SubServicesVecType;
    typedef std::map< FieldNameType, SubServicesVecType > SubServicesMapType;

    void initOnDummyObject( const FieldNameType& fieldName );
    void addField( const FieldNameType& fieldName, ::fwData::Object::sptr field );
    void swapField(const FieldNameType& fieldName, ::fwData::Object::sptr field);
    void removeField( const FieldNameType& fieldName );
    /// Slot: add objects
    void addFields(::fwData::Object::FieldsContainerType fields);

    /// Slot: change objects
    void changeFields(::fwData::Object::FieldsContainerType newFields, ::fwData::Object::FieldsContainerType oldFields);

    /// Slot: remove objects
    void removeFields(::fwData::Object::FieldsContainerType fields);

    ::fwServices::IService::sptr add( ::fwData::Object::sptr obj, ::fwRuntime::ConfigurationElement::sptr _elt );

private:

    std::string m_mode;
    bool m_dummyStopMode;
    ConfigurationType m_managerConfiguration;
    SubServicesMapType m_fieldsSubServices;
};

} // manager
} // ctrlSelection

#endif // __CTRLSELECTION_MANAGER_SFIELD_HPP__
