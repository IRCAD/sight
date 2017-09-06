/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUIQT_CONTAINER_QTCONTAINER_HPP__
#define __FWGUIQT_CONTAINER_QTCONTAINER_HPP__

#include "fwGuiQt/config.hpp"

#include <fwCore/base.hpp>

#include <fwGui/container/fwContainer.hpp>

#include <fwTools/Object.hpp>

#include <QPointer>

QT_BEGIN_NAMESPACE
class QLayout;
class QWidget;
QT_END_NAMESPACE

namespace fwGuiQt
{
namespace container
{

/**
 * @brief   Defines the QWidget container for UI.
 *
 */

class FWGUIQT_CLASS_API QtContainer : public ::fwGui::container::fwContainer
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (QtContainer)(::fwGui::container::fwContainer),
                                            (()),
                                            ::fwGui::factory::New< QtContainer > );

    FWGUIQT_API QtContainer(::fwGui::GuiBaseObject::Key key) noexcept;

    FWGUIQT_API virtual ~QtContainer() noexcept;

    FWGUIQT_API virtual void setQtContainer(QWidget* container);
    FWGUIQT_API virtual QWidget* getQtContainer();

    /**
     * @brief Assign a layout to the container widget. Previous layout and its children are deleted and the container
     *  widget take the ownership of the new layout
     *
     * @param The layout to set on the container widget
     */
    FWGUIQT_API virtual void setLayout(QLayout* const layout);

    FWGUIQT_API virtual void clean();
    FWGUIQT_API virtual void destroyContainer();
    FWGUIQT_API virtual bool isShownOnScreen();

    FWGUIQT_API virtual void setVisible(bool isVisible);
    FWGUIQT_API virtual void setEnabled(bool isEnabled);

private:

    QPointer<QWidget> m_container;
};

} // namespace container
} // namespace fwGuiQt

#endif /*__FWGUIQT_CONTAINER_QTCONTAINER_HPP__*/

