/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUIQT_QTMAINFRAME_HPP__
#define __FWGUIQT_QTMAINFRAME_HPP__

#include "fwGuiQt/config.hpp"

#include <QMainWindow>
#include <QPointer>

#include <functional>

namespace fwGuiQt
{

/**
 * @brief A qt panel used to control a VTK 2D Negatoscope view.
 *
 */
class QtMainFrame : public QMainWindow
{

Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    FWGUIQT_API QtMainFrame() noexcept;

    /// @brief Destructor.
    FWGUIQT_API virtual ~QtMainFrame() noexcept;

    typedef std::function<void ()> CloseCallback;
    FWGUIQT_API void setCloseCallback(CloseCallback fct);

protected:
    void closeEvent(QCloseEvent* event);

private:
    CloseCallback m_fctCloseCallback;
};

} // fwGuiQt

#endif // __FWGUIQT_QTMAINFRAME_HPP__
