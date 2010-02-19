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

    fwCoreServiceClassDefinitionsMacro ( (SimpleSwapperSrv)(::ctrlSelection::IManagerSrv::Baseclass) ) ;

    /// Constructor.  Do nothing.
    CTRLSELECTION_API SimpleSwapperSrv() throw() ;

    /// Destructor. Do nothing.
    CTRLSELECTION_API virtual ~SimpleSwapperSrv() throw() ;

protected:

    /// Implements starting method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Implements stopping method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void stopping()  throw ( ::fwTools::Failed );

    /// Implements configuring method derived from IService. Do nothing.
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
