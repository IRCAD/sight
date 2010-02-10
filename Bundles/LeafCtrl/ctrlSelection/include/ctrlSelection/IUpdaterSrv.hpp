/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _CTRLSELECTION_IUPDATERSRV_HPP_
#define _CTRLSELECTION_IUPDATERSRV_HPP_

#include <boost/tuple/tuple.hpp>

#include <fwData/Composite.hpp>

#include <fwServices/IService.hpp>

#include "ctrlSelection/config.hpp"


namespace ctrlSelection
{

/**
 * @class   IUpdaterSrv.
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class CTRLSELECTION_CLASS_API IUpdaterSrv : public ::fwServices::IService
{

public:

    enum ActionType {
        ADD,
        SWAP,
        REMOVE,
        DO_NOTHING,
        REMOVE_IF_PRESENT,
        ADD_OR_SWAP
    };

    fwCoreServiceClassDefinitionsMacro ( (IUpdaterSrv)(::fwServices::IService::Baseclass) ) ;

    ///@brief IUpdaterSrv constructor. Do nothing.
    CTRLSELECTION_API IUpdaterSrv();

    ///@brief IUpdaterSrv destructor. Do nothing.
    CTRLSELECTION_API virtual ~IUpdaterSrv();

protected:

    void updateComposite( ::fwData::Composite::sptr composite, ::fwData::Object::sptr _obj, std::string _compositeKey, ActionType _action );

    void configureManagedEvents(::fwRuntime::ConfigurationElement::sptr configuration);

    typedef std::vector < ::boost::tuple< std::string, std::string, std::string, ActionType > > ManagedEvents;

    ManagedEvents m_managedEvents;
};

}

#endif // _CTRLSELECTION_IUPDATERSRV_HPP_
