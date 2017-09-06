/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUIQT_CONTAINER_QTMENUCONTAINER_HPP__
#define __FWGUIQT_CONTAINER_QTMENUCONTAINER_HPP__

#include <QPointer>

#include <fwCore/base.hpp>
#include <fwTools/Object.hpp>

#include <fwGui/container/fwMenu.hpp>

#include "fwGuiQt/config.hpp"


QT_BEGIN_NAMESPACE
class QMenu;
QT_END_NAMESPACE


namespace fwGuiQt
{
namespace container
{

/**
 * @brief   Defines the Qt menu bar container for IHM.
 * @class   QtMenuContainer
 *
 * @date    2009-2010.
 *
 */
class FWGUIQT_CLASS_API QtMenuContainer : public ::fwGui::container::fwMenu
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (QtMenuContainer)(::fwGui::container::fwMenu),
                                            (()),
                                            ::fwGui::factory::New< QtMenuContainer > );

    FWGUIQT_API QtMenuContainer(::fwGui::GuiBaseObject::Key key) noexcept;

    FWGUIQT_API virtual ~QtMenuContainer() noexcept;


    FWGUIQT_API virtual void clean();
    FWGUIQT_API virtual void destroyContainer();

    FWGUIQT_API virtual void setQtMenu(QMenu *menu);
    FWGUIQT_API virtual QMenu* getQtMenu();

private:

    QPointer<QMenu> m_menu;
};

} // namespace container
} // namespace fwGuiQt

#endif /*__FWGUIQT_CONTAINER_QTMENUCONTAINER_HPP__*/


