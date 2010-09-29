/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIQT_CONTAINER_QTCONTAINER_HPP_
#define _FWGUIQT_CONTAINER_QTCONTAINER_HPP_

#include <QPointer>

#include <fwCore/base.hpp>
#include <fwTools/Object.hpp>

#include <fwGui/container/fwContainer.hpp>

#include "fwGuiQt/config.hpp"


QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE


namespace fwGuiQt
{
namespace container
{

/**
 * @brief   Defines the QWidget container for IHM.
 * @class   QtContainer.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUIQT_CLASS_API QtContainer : public ::fwGui::container::fwContainer
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (QtContainer)(::fwGui::container::fwContainer), (()), new QtContainer );

    /**
     * @brief   Constructor.
     */
    FWGUIQT_API QtContainer() throw() ;

    /**
     * @brief   Destructor. Do nothing.
     */
    FWGUIQT_API virtual ~QtContainer() throw() ;


    FWGUIQT_API virtual void clean();
    FWGUIQT_API virtual void destroyContainer();

    FWGUIQT_API virtual void setQtContainer(QWidget *container);
    FWGUIQT_API virtual QWidget* getQtContainer();
    FWGUIQT_API virtual bool isShownOnScreen();


private :

    QPointer<QWidget> m_container;
};

} // namespace container
} // namespace fwGuiQt

#endif /*_FWGUIQT_CONTAINER_QTCONTAINER_HPP_*/


