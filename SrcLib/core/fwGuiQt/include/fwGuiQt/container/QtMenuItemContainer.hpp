/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUIQT_CONTAINER_QTMENUITEMCONTAINER_HPP__
#define __FWGUIQT_CONTAINER_QTMENUITEMCONTAINER_HPP__

#include <QPointer>

#include <fwCore/base.hpp>
#include <fwTools/Object.hpp>

#include <fwGui/container/fwMenuItem.hpp>

#include "fwGuiQt/config.hpp"

QT_BEGIN_NAMESPACE
class QAction;
QT_END_NAMESPACE

namespace fwGuiQt
{
namespace container
{

/**
 * @brief   Defines the Qt menuItem bar container for IHM.
 * @class   QtMenuItemContainer
 *
 * @date    2009-2010.
 *
 */
class FWGUIQT_CLASS_API QtMenuItemContainer : public ::fwGui::container::fwMenuItem
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (QtMenuItemContainer)(::fwGui::container::fwMenuItem),
                                            (()),
                                            ::fwGui::factory::New< QtMenuItemContainer > );

    FWGUIQT_API QtMenuItemContainer(::fwGui::GuiBaseObject::Key key) noexcept;

    FWGUIQT_API virtual ~QtMenuItemContainer() noexcept;


    FWGUIQT_API virtual void clean();
    FWGUIQT_API virtual void destroyContainer();

    FWGUIQT_API virtual void setQtMenuItem(QAction* menuItem);
    FWGUIQT_API virtual QAction* getQtMenuItem();

private:

    QPointer<QAction> m_menuItem;
};

} // namespace container
} // namespace fwGuiQt

#endif /*__FWGUIQT_CONTAINER_QTMENUITEMCONTAINER_HPP__*/


