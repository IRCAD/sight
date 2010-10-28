/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIQT_MAINFRAME_HPP_
#define _FWGUIQT_MAINFRAME_HPP_


#include <QPointer>
#include <QMainWindow>

#include <boost/function.hpp>

#include "fwGuiQt/config.hpp"

namespace fwGuiQt
{

/**
 * @brief A qt panel used to control a VTK 2D Negatoscope view.
 * @author IRCAD (Research and Development Team).
 */
class QtMainFrame : public QMainWindow
{

    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param parent The parent window.
     * @param id     The window ID.
     */
    FWGUIQT_API QtMainFrame() throw();


    /// @brief Destructor.
    FWGUIQT_API virtual ~QtMainFrame() throw();

    typedef ::boost::function0<void> CloseCallback;
    FWGUIQT_API void setCloseCallback(CloseCallback fct);


protected:
    void closeEvent(QCloseEvent *event);

private:
    CloseCallback m_fctCloseCallback;
};

} // fwGuiQt

#endif // _FWGUIQT_MAINFRAME_HPP_
