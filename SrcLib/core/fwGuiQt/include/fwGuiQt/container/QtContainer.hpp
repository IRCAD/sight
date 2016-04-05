/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUIQT_CONTAINER_QTCONTAINER_HPP__
#define __FWGUIQT_CONTAINER_QTCONTAINER_HPP__

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
 * @class   QtContainer
 *
 * @date    2009-2010.
 *
 */
class FWGUIQT_CLASS_API QtContainer : public ::fwGui::container::fwContainer
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (QtContainer)(::fwGui::container::fwContainer),
                                            (()),
                                            ::fwGui::factory::New< QtContainer > );

    FWGUIQT_API QtContainer(::fwGui::GuiBaseObject::Key key) throw();

    FWGUIQT_API virtual ~QtContainer() throw();


    FWGUIQT_API virtual void clean();
    FWGUIQT_API virtual void destroyContainer();

    FWGUIQT_API virtual void setQtContainer(QWidget *container);
    FWGUIQT_API virtual QWidget* getQtContainer();
    FWGUIQT_API virtual bool isShownOnScreen();

    FWGUIQT_API virtual void setVisible(bool isVisible);
    FWGUIQT_API virtual void setEnabled(bool isEnabled);

private:

    QPointer<QWidget> m_container;
};

} // namespace container
} // namespace fwGuiQt

#endif /*__FWGUIQT_CONTAINER_QTCONTAINER_HPP__*/


