/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIQT_ACTIONCALLBACK_HPP_
#define _FWGUIQT_ACTIONCALLBACK_HPP_


#include <QObject>

#include <fwGui/ActionCallbackBase.hpp>

#include "fwGuiQt/config.hpp"


namespace fwGuiQt
{

/**
 * @brief   Defines the menu item callback.
 * @class   ActionCallback.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUIQT_CLASS_API ActionCallback :  public QObject, public ::fwGui::ActionCallbackBase
{

    Q_OBJECT

public :

    fwCoreClassDefinitionsWithFactoryMacro( (ActionCallback)(::fwGui::ActionCallbackBase), (()), new ActionCallback );

    /**
     * @brief   Constructor.
     */
    FWGUIQT_API ActionCallback() ;

    /**
     * @brief   Destructor. Do nothing.
     */
    FWGUIQT_API virtual ~ActionCallback() ;

    /**
     * @brief  Qt slot.
     */
    public slots:
    void executeQt(bool checked = false);

};

} // namespace fwGuiQt

#endif /*_FWGUIQT_ACTIONCALLBACK_HPP_*/


