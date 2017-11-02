/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUIQT_LAYOUTMANAGER_LINELAYOUTMANAGER_HPP__
#define __FWGUIQT_LAYOUTMANAGER_LINELAYOUTMANAGER_HPP__

#include "fwGuiQt/config.hpp"
#include "fwGuiQt/container/QtContainer.hpp"

#include <fwCore/base.hpp>

#include <fwGui/layoutManager/LineLayoutManagerBase.hpp>

namespace fwGui
{

/**
 * @brief   Defines the line layout manager.
 */
class FWGUIQT_CLASS_API LineLayoutManager : public ::fwGui::layoutManager::LineLayoutManagerBase
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (LineLayoutManager)(::fwGui::layoutManager::LineLayoutManagerBase),
                                            (()),
                                            ::fwGui::factory::New< LineLayoutManager > );

    FWGUIQT_API LineLayoutManager(::fwGui::GuiBaseObject::Key key);

    FWGUIQT_API virtual ~LineLayoutManager();

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
};

} // namespace fwGui

#endif /*__FWGUIQT_LAYOUTMANAGER_LINELAYOUTMANAGER_HPP__*/

