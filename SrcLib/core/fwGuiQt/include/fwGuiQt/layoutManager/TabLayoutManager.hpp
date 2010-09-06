/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIQT_LAYOUTMANAGER_TABLAYOUTMANAGER_HPP_
#define _FWGUIQT_LAYOUTMANAGER_TABLAYOUTMANAGER_HPP_

#include <QPointer>

#include <fwCore/base.hpp>

#include <fwGui/layoutManager/TabLayoutManagerBase.hpp>

#include "fwGuiQt/container/QtContainer.hpp"
#include "fwGuiQt/config.hpp"


QT_BEGIN_NAMESPACE
class QTabWidget;
QT_END_NAMESPACE


namespace fwGui
{

/**
 * @brief   Defines the tab layout manager.
 * @class   TabLayoutManager.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUIQT_CLASS_API TabLayoutManager : public ::fwGui::layoutManager::TabLayoutManagerBase
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (TabLayoutManager)(::fwGui::layoutManager::TabLayoutManagerBase), (()), new TabLayoutManager );

    /// Constructor. Do nothing.
    FWGUIQT_API TabLayoutManager();

    /// Destructor. Do nothing.
    FWGUIQT_API virtual ~TabLayoutManager();

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

    // Layout manager of this view
    QPointer<QTabWidget> m_tabWidget;
};

} // namespace fwGui

#endif /*_FWGUIQT_LAYOUTMANAGER_TABLAYOUTMANAGER_HPP_*/


