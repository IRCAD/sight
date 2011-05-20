/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _CTRLSELECTION_TRANSLATEUPDATER_HPP_
#define _CTRLSELECTION_TRANSLATEUPDATER_HPP_

#include "ctrlSelection/config.hpp"
#include "ctrlSelection/IUpdaterSrv.hpp"

namespace ctrlSelection
{

namespace updater
{

/**
 * @class   TranslateUpdater
 * @brief   Update the composite related to this service according to the receiving composite message and translate the composite keys.
 * @author IRCAD (Research and Development Team).
 * @date    2011.
 */
class CTRLSELECTION_CLASS_API TranslateUpdater : public ::ctrlSelection::IUpdaterSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (TranslateUpdater)(::ctrlSelection::IUpdaterSrv) ) ;

    /// Constructor.  Do nothing.
    CTRLSELECTION_API TranslateUpdater() throw() ;

    /// Destructor. Do nothing.
    CTRLSELECTION_API virtual ~TranslateUpdater() throw() ;

protected:

    /// Implements starting method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Implements stopping method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void stopping()  throw ( ::fwTools::Failed );

    /**
     * @brief Configure the services : declare the event to react.
     * @verbatim
         <service uid="myUpdater" implementation="::ctrlSelection::updater::TranslateUpdater" type="::ctrlSelection::IUpdaterSrv" autoComChannel="no"  >
            <translate fromKey="myObject1Key1" toKey="myObject1Key2" fromUID="myComposite" />
            <translate fromKey="myObject2Key1" toKey="myObject2Key2" fromUID="myComposite" />
        </service>
      @endverbatim
     * @see IUpdaterSrv::configureManagedEvents(::fwRuntime::ConfigurationElement::sptr configuration);
     */
    CTRLSELECTION_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// Implements reconfiguring method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void reconfiguring()  throw ( ::fwTools::Failed );

    /// Implements updating method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void updating() throw ( ::fwTools::Failed );

    /// Implements info method derived from IService. Print classname.
    CTRLSELECTION_API virtual void info( std::ostream &_sstream );

    CTRLSELECTION_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );

private:

    /// Managed translation : fromUID, fromKey, toKey
    typedef std::vector < ::boost::tuple< std::string, std::string, std::string > > ManagedTranslations;
    /// List of the managed translations
    ManagedTranslations m_managedTranslations;
};

} // updater
} // ctrlSelection

#endif // _CTRLSELECTION_TRANSLATEUPDATER_HPP_
