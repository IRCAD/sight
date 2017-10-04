/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUIQT_LAYOUTMANAGER_TABLAYOUTMANAGER_HPP__
#define __FWGUIQT_LAYOUTMANAGER_TABLAYOUTMANAGER_HPP__

#include "fwGuiQt/config.hpp"
#include "fwGuiQt/container/QtContainer.hpp"

#include <fwCore/base.hpp>

#include <fwGui/layoutManager/TabLayoutManagerBase.hpp>

#include <QPointer>

QT_BEGIN_NAMESPACE
class QTabWidget;
QT_END_NAMESPACE

namespace fwGui
{

/**
 * @brief   Defines the tab layout manager.
 */
class FWGUIQT_CLASS_API TabLayoutManager : public ::fwGui::layoutManager::TabLayoutManagerBase
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (TabLayoutManager)(::fwGui::layoutManager::TabLayoutManagerBase),
                                            (()),
                                            ::fwGui::factory::New< TabLayoutManager > );

    FWGUIQT_API TabLayoutManager(::fwGui::GuiBaseObject::Key key);

    FWGUIQT_API virtual ~TabLayoutManager();

    /**
     * @brief Instantiate layout with parent container.
     * @pre LayoutManager must be initialized before.
     * @pre parent containers must be instanced.
     */
    FWGUIQT_API virtual void createLayout( ::fwGui::container::fwContainer::sptr parent ) override;

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    FWGUIQT_API virtual void destroyLayout() override;

private:

    ::fwGuiQt::container::QtContainer::sptr m_parentContainer;

    // Layout manager of this view
    QPointer<QTabWidget> m_tabWidget;
};

} // namespace fwGui

#endif /*__FWGUIQT_LAYOUTMANAGER_TABLAYOUTMANAGER_HPP__*/

