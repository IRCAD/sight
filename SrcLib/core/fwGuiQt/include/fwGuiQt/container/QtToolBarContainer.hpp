/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIQT_CONTAINER_QTTOOLBARCONTAINER_HPP_
#define _FWGUIQT_CONTAINER_QTTOOLBARCONTAINER_HPP_

#include <QPointer>

#include <fwCore/base.hpp>
#include <fwTools/Object.hpp>

#include <fwGui/fwToolBar.hpp>

#include "fwGuiQt/config.hpp"


QT_BEGIN_NAMESPACE
class QToolBar;
QT_END_NAMESPACE


namespace fwGuiQt
{
namespace container
{

/**
 * @brief   Defines the Qt toolBar bar container for IHM.
 * @class   QtToolBarContainer.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUIQT_CLASS_API QtToolBarContainer : public ::fwGui::fwToolBar
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (QtToolBarContainer)(::fwGui::fwToolBar), (()), new QtToolBarContainer );

    /**
     * @brief   Constructor.
     */
    FWGUIQT_API QtToolBarContainer() throw() ;

    /**
     * @brief   Destructor. Do nothing.
     */
    FWGUIQT_API virtual ~QtToolBarContainer() throw() ;


    FWGUIQT_API virtual void clean();
    FWGUIQT_API virtual void destroyContainer();

    FWGUIQT_API virtual void setQtToolBar(QToolBar *toolBar);
    FWGUIQT_API virtual QToolBar* getQtToolBar();

private :

    QPointer<QToolBar> m_toolBar;
};

} // namespace container
} // namespace fwGuiQt

#endif /*_FWGUIQT_CONTAINER_QTTOOLBARCONTAINER_HPP_*/


