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

namespace manager
{

/**
 * @class  SwapperSrv.
 * @author IRCAD (Research and Development Team).

 * @date   2007-2009.
 */
class CTRLSELECTION_CLASS_API SwapperSrv : public ::ctrlSelection::IManagerSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (SwapperSrv)(::ctrlSelection::IManagerSrv::Baseclass) ) ;

    /// Constructor.  Do nothing.
    CTRLSELECTION_API SwapperSrv() throw() ;

    /// Destructor. Do nothing.
    CTRLSELECTION_API virtual ~SwapperSrv() throw() ;

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

protected:

    class SubService;

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

        SPTR(::fwTools::Object) m_dummy;
        ConfigurationType m_config;
        WPTR(::fwServices::IService) m_service;
    };

    typedef std::vector<SubService> SubServicesVecType;
    typedef std::map< ObjectIdType, SubServicesVecType > SubServicesMapType ;

    void configureObject( ConfigurationType conf );

private:
    bool m_dummyStopMode;
    ConfigurationType m_managerConfiguration;
    SubServicesMapType m_objectsSubServices;

};

} // manager
} // ctrlSelection

#endif // _CTRLSELECTION_MANAGER_SWAPPER_HPP_
