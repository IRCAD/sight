/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIQT_LAYOUTMANAGER_CARDINALLAYOUTMANAGER_HPP_
#define _FWGUIQT_LAYOUTMANAGER_CARDINALLAYOUTMANAGER_HPP_

#include <QPointer>

#include <fwCore/base.hpp>

#include <fwGui/layoutManager/CardinalLayoutManagerBase.hpp>

#include "fwGuiQt/container/QtContainer.hpp"
#include "fwGuiQt/config.hpp"

QT_BEGIN_NAMESPACE
class QMainWindow;
QT_END_NAMESPACE


namespace fwGui
{

/**
 * @brief   Defines the cardinal layout manager.
 * @class   CardinalLayoutManager.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUIQT_CLASS_API CardinalLayoutManager : public ::fwGui::layoutManager::CardinalLayoutManagerBase
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (CardinalLayoutManager)(::fwGui::layoutManager::CardinalLayoutManagerBase), (()), new CardinalLayoutManager );

    /// Constructor. Do nothing.
    FWGUIQT_API CardinalLayoutManager();

    /// Destructor. Do nothing.
    FWGUIQT_API virtual ~CardinalLayoutManager();

    /**
     * @brief Instantiate layout with parent container.
     * @pre LayoutManager must be initialized before.
     * @pre parent containers must be instanced.
     */
    FWGUIQT_API virtual void createLayout( ::fwGui::container::fwContainer::sptr parent );

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    FWGUIQT_API virtual void destroyLayout();

private:

    ::fwGuiQt::container::QtContainer::sptr m_parentContainer;

    /// Layout manager of this view
    QPointer<QMainWindow> m_qtWindow;
};

} // namespace fwGui

#endif /*_FWGUIQT_LAYOUTMANAGER_CARDINALLAYOUTMANAGER_HPP_*/


