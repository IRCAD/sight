/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _CTRLSELECTION_MANAGER_SIMPLESWAPPER_HPP_
#define _CTRLSELECTION_MANAGER_SIMPLESWAPPER_HPP_

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwServices/IService.hpp>
#include "ctrlSelection/config.hpp"
#include "ctrlSelection/IManagerSrv.hpp"

namespace ctrlSelection
{

namespace manager
{

/**
 * @class  SimpleSwapperSrv.
 * @author IRCAD (Research and Development Team).

 * @date   2007-2009.
 */
class CTRLSELECTION_CLASS_API SimpleSwapperSrv : public ::ctrlSelection::IManagerSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (SimpleSwapperSrv)(::ctrlSelection::IManagerSrv) ) ;

    /// Constructor.  Do nothing.
    CTRLSELECTION_API SimpleSwapperSrv() throw() ;

    /// Destructor. Do nothing.
    CTRLSELECTION_API virtual ~SimpleSwapperSrv() throw() ;

protected:

    /// Implements starting method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Implements stopping method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void stopping()  throw ( ::fwTools::Failed );

    /**
    * @brief Implements configuring method derived from IService. .
    *
    * Sample of declaration configuration for a simple swapper service
    *
    * @verbatim
     <service uid="myManagerImageSelected" implementation="::ctrlSelection::manager::SimpleSwapperSrv" type="::ctrlSelection::IManagerSrv" autoComChannel="yes">
            <config>
                <object id="MySelectedImage" dummyObjectUID="DummyImage" type="::fwData::Image" >
                    <service uid="myService" />
                    <service uid="myService2" />
                </object>
            </config>
        </service>
     @endverbatim
    * With:
    * @li id: Object id of the composite where services will be attached.(In your sample myService).
    * @li dummyObjectUID: (mandatory) object uid on dummy object use to swap the attached services when the object is deleted
    * (in our sample myService and myService2 will be attached to DummyImage if MySelectedImage is deleted).
    * @li <service uid= "myService" />: declares the service attached to the MySelectedImage object. its will be swapped when the MySelectedImage object data will be changed
    * It can have several services on the same object.


    */
    CTRLSELECTION_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// Implements reconfiguring method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void reconfiguring()  throw ( ::fwTools::Failed );

    /// Implements updating method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void updating() throw ( ::fwTools::Failed );

    /// Implements info method derived from IService. Print classname.
    CTRLSELECTION_API virtual void info( std::ostream &_sstream );

    CTRLSELECTION_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );


    CTRLSELECTION_API void addedObjects( ::fwData::Composite::sptr _composite );
    CTRLSELECTION_API void swappedObjects( ::fwData::Composite::sptr _composite );
    CTRLSELECTION_API void removedObjects( ::fwData::Composite::sptr _composite );


    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    typedef std::string ObjectIdType;

    class SubService
    {
    public:
        SubService()
        { }
        ~SubService()
        { }
        ::std::string m_serviceUID;
        ::std::string m_dummyObjectUID;
    };

    typedef std::vector< SPTR(SubService) > SubServicesVecType;
    typedef std::map< ObjectIdType, SubServicesVecType > SubServicesMapType ;


private:

    ConfigurationType m_managerConfiguration;
    SubServicesMapType m_objectsSubServices;

};

} // manager
} // ctrlSelection

#endif // _CTRLSELECTION_MANAGER_SIMPLESWAPPER_HPP_
