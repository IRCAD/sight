/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUIQT_QTMAINFRAME_HPP__
#define __FWGUIQT_QTMAINFRAME_HPP__


#include <QPointer>
#include <QMainWindow>

#include <boost/function.hpp>

#include "fwGuiQt/config.hpp"

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

    typedef ::boost::function0<void> CloseCallback;
    FWGUIQT_API void setCloseCallback(CloseCallback fct);


protected:
    void closeEvent(QCloseEvent *event);

private:
    CloseCallback m_fctCloseCallback;
};

} // fwGuiQt

#endif // __FWGUIQT_QTMAINFRAME_HPP__
