/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIQT_CONTAINER_QTMENUITEMCONTAINER_HPP_
#define _FWGUIQT_CONTAINER_QTMENUITEMCONTAINER_HPP_

#include <QPointer>

#include <fwCore/base.hpp>
#include <fwTools/Object.hpp>

#include <fwGui/fwMenuItem.hpp>

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
 * @class   QtMenuItemContainer.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUIQT_CLASS_API QtMenuItemContainer : public ::fwGui::fwMenuItem
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (QtMenuItemContainer)(::fwGui::fwMenuItem), (()), new QtMenuItemContainer );

    /**
     * @brief   Constructor.
     */
    FWGUIQT_API QtMenuItemContainer() throw() ;

    /**
     * @brief   Destructor. Do nothing.
     */
    FWGUIQT_API virtual ~QtMenuItemContainer() throw() ;


    FWGUIQT_API virtual void clean();
    FWGUIQT_API virtual void destroyContainer();

    FWGUIQT_API virtual void setQtMenuItem(QAction* menuItem);
    FWGUIQT_API virtual QAction* getQtMenuItem();

private :

    QPointer<QAction> m_menuItem;
};

} // namespace container
} // namespace fwGuiQt

#endif /*_FWGUIQT_CONTAINER_QTMENUITEMCONTAINER_HPP_*/


