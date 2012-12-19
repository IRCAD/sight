/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _CTRLSELECTION_BOOKMARKSRV_HPP_
#define _CTRLSELECTION_BOOKMARKSRV_HPP_

#include <fwServices/IService.hpp>
#include <fwServices/IController.hpp>

#include "ctrlSelection/config.hpp"

namespace ctrlSelection
{


/**
 * @class  BookmarkSrv.
 * @brief  This service bookmark its object with the name given in configuration.
 * @author IRCAD (Research and Development Team).

 * @date   2011.
 */
class CTRLSELECTION_CLASS_API BookmarkSrv : public ::fwServices::IController
{

public :

    fwCoreServiceClassDefinitionsMacro ( (BookmarkSrv)(::fwServices::IController) ) ;

    CTRLSELECTION_API BookmarkSrv() throw() ;

    CTRLSELECTION_API virtual ~BookmarkSrv() throw() ;

protected:

    /// Add the object in bookmark.
    CTRLSELECTION_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Remove the object from bookmark
    CTRLSELECTION_API virtual void stopping()  throw ( ::fwTools::Failed );

    /// Do nothing
    CTRLSELECTION_API virtual void swapping()  throw ( ::fwTools::Failed );

    CTRLSELECTION_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// Do nothing
    CTRLSELECTION_API virtual void reconfiguring()  throw ( ::fwTools::Failed );

    /// Do nothing
    CTRLSELECTION_API virtual void updating() throw ( ::fwTools::Failed );

    CTRLSELECTION_API virtual void info( std::ostream &_sstream );

    /// Do nothing
    CTRLSELECTION_API virtual void receiving( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );

private:
    std::string m_bookmarkName;
};

} // ctrlSelection

#endif // _CTRLSELECTION_BOOKMARKSRV_HPP_
