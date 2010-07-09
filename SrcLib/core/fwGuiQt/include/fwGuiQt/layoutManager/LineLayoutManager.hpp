/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIQT_LAYOUTMANAGER_LINELAYOUTMANAGER_HPP_
#define _FWGUIQT_LAYOUTMANAGER_LINELAYOUTMANAGER_HPP_

#include <fwCore/base.hpp>

#include <fwGui/layoutManager/LineLayoutManagerBase.hpp>

#include "fwGuiQt/container/QtContainer.hpp"
#include "fwGuiQt/config.hpp"

namespace fwGui
{

/**
 * @brief   Defines the line layout manager.
 * @class   LineLayoutManager.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUIQT_CLASS_API LineLayoutManager : public ::fwGui::layoutManager::LineLayoutManagerBase
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (LineLayoutManager)(::fwGui::layoutManager::LineLayoutManagerBase), (()), new LineLayoutManager );

    /// Constructor. Do nothing.
    FWGUIQT_API LineLayoutManager();

    /// Destructor. Do nothing.
    FWGUIQT_API virtual ~LineLayoutManager();

    /**
     * @brief Instantiate layout with parent container.
     * @pre LayoutManager must be initialized before.
     * @pre parent containers must be instanced.
     */
    FWGUIQT_API virtual void createLayout( ::fwGui::fwContainer::sptr parent );

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    FWGUIQT_API virtual void destroyLayout();

private:
    ::fwGuiQt::container::QtContainer::sptr m_parentContainer;
};

} // namespace fwGui

#endif /*_FWGUIQT_LAYOUTMANAGER_LINELAYOUTMANAGER_HPP_*/


