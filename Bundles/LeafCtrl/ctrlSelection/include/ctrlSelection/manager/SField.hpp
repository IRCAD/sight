/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _CTRLSELECTION_MANAGER_SFIELD_HPP_
#define _CTRLSELECTION_MANAGER_SFIELD_HPP_

#include <fwData/Object.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/ObjectMsg.hpp>

#include "ctrlSelection/config.hpp"
#include "ctrlSelection/IManagerSrv.hpp"

namespace ctrlSelection
{
namespace manager
{

/**
 * @class  SField.
 * @brief  This services is a manager which starts, stops or swaps services on field contained in a composite when
 * it receive specific message (mainly sent by updater).
 * @author IRCAD (Research and Development Team).

 * @date   2007-2009.
 */
class CTRLSELECTION_CLASS_API SField : public ::ctrlSelection::IManagerSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (SField)(::ctrlSelection::IManagerSrv) ) ;

    /// Constructor.  Do nothing.
    CTRLSELECTION_API SField() throw() ;

    /// Destructor. Do nothing.
    CTRLSELECTION_API virtual ~SField() throw() ;

protected:

    /// Implements starting method derived from IService. Starts the managed services if their fields are in the composite.
    /// If the mode is "dummy", starts the the managed services on dummy fields if their fields are not in the composite.
    CTRLSELECTION_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Implements stopping method derived from IService. Stops and erases all the managed services.
    CTRLSELECTION_API virtual void stopping()  throw ( ::fwTools::Failed );

    /**
    * @brief Implements configuring method derived from IService. .
    *
    * Sample of declaration configuration for a simple swapper service
    *
    * @verbatim
        <service uid="myFieldManager" implementation="::ctrlSelection::manager::SField" type="::ctrlSelection::IManagerSrv" autoComChannel="yes" >
            <mode type="dummy" />
            <config>
                <field id="TFSelection" type="::fwData::TransferFunction" >
                    <service uid="myServices" implementation="..." type="..." autoComChannel="yes" />
                </field>
                <field id="AxialSliceIndex" type="::fwData::Integer" >
                    <service uid="myServices2" implementation="..." type="..." autoComChannel="yes" />
                </field>
            </config>
        </service>
    @endverbatim
    * With:
    * @li mode : must be "stop" or "dummy". The dummy mode doesn't stop the services when its attached field is deleted but swap it on a dummy field.
    * @li the fields and services tags are defined as same as the configuration of fields and services.
    */
    CTRLSELECTION_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// Implements reconfiguring method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void reconfiguring()  throw ( ::fwTools::Failed );

    /// Implements updating method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void updating() throw ( ::fwTools::Failed );

    /// Implements info method derived from IService. Print classname.
    CTRLSELECTION_API virtual void info( std::ostream &_sstream );

    /// Reacts on specifics event (ADDED_FIELDS, REMOVED_FIELDS and SWAPPED_FIELDS) and start, stop or swap the managed services
    /// on the fields defined in the message or dummy fields
    CTRLSELECTION_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );


    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    typedef ::fwData::Object::FieldNameType FieldNameType;
    typedef ::fwServices::ObjectMsg::ModifiedFieldsContainerType ModifiedFieldsContainerType;

    class SubService
    {
    public:

        SubService()
        {
            m_hasComChannel = false;
        }

        ~SubService()
        { }

        SPTR (::fwServices::IService) getService()
                    { return m_service.lock(); }

        SPTR (::fwServices::IService) getComChannel()
                            { return m_comChannel.lock(); }

        ::fwTools::Object::sptr m_dummy;
        ConfigurationType m_config;
        WPTR(::fwServices::IService) m_service;
        WPTR(::fwServices::ComChannelService) m_comChannel;
        bool m_hasComChannel;
    };

    typedef std::vector< SPTR(SubService) > SubServicesVecType;
    typedef std::map< FieldNameType, SubServicesVecType > SubServicesMapType ;


    void initOnDummyObject( const FieldNameType& fieldName );
    void addFields( const ModifiedFieldsContainerType& fields );
    void addField( const FieldNameType& fieldName, ::fwData::Object::sptr field );
    void swapFields( const ModifiedFieldsContainerType& fields );
    void swapField(const FieldNameType& fieldName, ::fwTools::Object::sptr field);
    void removeFields( const ModifiedFieldsContainerType& fields );
    void removeField( const FieldNameType& fieldName );

    ::fwServices::IService::sptr add( ::fwTools::Object::sptr obj , ::fwRuntime::ConfigurationElement::sptr _elt );

private:

    std::string m_mode;
    bool m_dummyStopMode;
    ConfigurationType m_managerConfiguration;
    SubServicesMapType m_fieldsSubServices;
};

} // manager
} // ctrlSelection

#endif // _CTRLSELECTION_MANAGER_SFIELD_HPP_
