/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLSELECTION_BOOKMARKSRV_HPP__
#define __CTRLSELECTION_BOOKMARKSRV_HPP__

#include "ctrlSelection/config.hpp"

#include <fwServices/IController.hpp>
#include <fwServices/IService.hpp>

namespace ctrlSelection
{

/**
 * @brief  This service bookmark its object with the name given in configuration.
 */
class CTRLSELECTION_CLASS_API BookmarkSrv : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro( (BookmarkSrv)(::fwServices::IController) );

    CTRLSELECTION_API BookmarkSrv() noexcept;

    CTRLSELECTION_API virtual ~BookmarkSrv() noexcept;

protected:

    /// Adds the object in bookmark.
    CTRLSELECTION_API virtual void starting() override;

    /// Removes the object from bookmark
    CTRLSELECTION_API virtual void stopping() override;

    /// Do nothing
    CTRLSELECTION_API virtual void swapping() override;

    /**
     * @code{.xml}
       <service impl="::ctrlSelection::BookmarkSrv" type="::fwServices::IController" autoConnect="no">
           <bookmark fromString="..." name="..." />
       </service>
       @endcode
     * - \b fromString : prefix of the bookmark key
     * - \b name : name of the bookmark
     * The prefix and the name are concatenated as 'prefix_name'.
     */
    CTRLSELECTION_API virtual void configuring() override;

    /// Do nothing
    CTRLSELECTION_API virtual void reconfiguring() override;

    /// Do nothing
    CTRLSELECTION_API virtual void updating() override;

    CTRLSELECTION_API virtual void info( std::ostream& _sstream ) override;

private:
    std::string m_bookmarkName;
};

} // ctrlSelection

#endif // __CTRLSELECTION_BOOKMARKSRV_HPP__
