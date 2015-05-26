/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __TRACKER_ITRACKER_HPP__
#define __TRACKER_ITRACKER_HPP__

#include <fwServices/config.hpp>
#include <fwServices/IService.hpp>

namespace tracker
{

/**
 * @brief   This interface defines Tracker service API.
 * @class   ITracker
 */
class FWSERVICES_CLASS_API ITracker : public ::fwServices::IService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (ITracker)(::fwServices::IService) );

protected:

    ///@brief ITracker constructor. Do nothing.
    ITracker()
    {
    }

    ///@brief ITracker destructor. Do nothing.
    virtual ~ITracker()
    {
    }

};
} //tracker

#endif // __TRACKER_ITRACKER_HPP__
