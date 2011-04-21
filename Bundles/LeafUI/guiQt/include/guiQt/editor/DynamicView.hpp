/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUIQT_EDITOR_DYNAMICVIEW_HPP_
#define _GUIQT_EDITOR_DYNAMICVIEW_HPP_

#include <map>

#include <QObject>
#include <QPointer>

#include <fwTools/Failed.hpp>

#include <gui/view/IView.hpp>

#include <fwServices/AppConfigManager.hpp>
#include <fwGuiQt/container/QtContainer.hpp>


#include "guiQt/config.hpp"


class QTabWidget;

namespace guiQt
{
namespace editor
{
/**
 * @class   DynamicView
 * @brief   This editor manages tabs. It receive message with NEW_CONFIGURATION_HELPER event containing the view config id.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 *
 * @note The ::gui::action::ConfigActionSrvWithKeySendingConfigTemplate action sends message to be receive by the editor.
 */
class GUIQT_CLASS_API DynamicView : public QObject, public ::gui::view::IView
{
    Q_OBJECT

public :

    fwCoreServiceClassDefinitionsMacro ( (DynamicView)(::gui::view::IView) ) ;

    /// Constructor. Do nothing.
    GUIQT_API DynamicView() throw() ;

    /// Destructor. Do nothing.
    GUIQT_API virtual ~DynamicView() throw() ;

protected:

    /**
     * @brief Install the container.
     */
    virtual void starting() throw(::fwTools::Failed);

    /**
     * @brief Destroy the container.
     */
    virtual void stopping() throw(::fwTools::Failed);

    /**
     * @brief Analyses received message with NEW_CONFIGURATION_HELPER helper.
     * Creates the view defines by the config given in message.
     */
    virtual void updating( ::boost::shared_ptr< const fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed);

    /**
     * @brief Update
     *
     */
    virtual void updating() throw(::fwTools::Failed);

    /**
     * @brief Swap
     *
     * Update the editors.
     */
    virtual void swapping() throw(::fwTools::Failed);

    /**
     * @brief Configure the editor
     *
     */
    virtual void configuring() throw(fwTools::Failed);

    virtual void info( std::ostream &_sstream ) ;

    struct DynamicViewInfo
    {
        ::fwGuiQt::container::QtContainer::sptr   container;
        ::fwServices::AppConfigManager::sptr helper;
        std::string wid;
        std::string title;
        bool closable;
    };

    typedef std::map< QWidget* , DynamicViewInfo > DynamicViewInfoMapType;

    QPointer<QTabWidget> m_tabWidget;
    QPointer<QWidget> m_currentWidget;

    std::map< std::string, unsigned int > m_titleToCount;

    DynamicViewInfoMapType m_dynamicInfoMap;
    bool m_dynamicConfigStartStop;

protected slots:

    void closeTab( int index, bool forceClose );

    void closeTabSignal(int index);

    void changedTab(int index);

};

}//namespace editor
} // guiQt

#endif /*_GUIQT_EDITOR_DYNAMICVIEW_HPP_*/


