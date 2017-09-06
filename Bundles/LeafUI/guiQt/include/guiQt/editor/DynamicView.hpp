/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GUIQT_EDITOR_DYNAMICVIEW_HPP__
#define __GUIQT_EDITOR_DYNAMICVIEW_HPP__

#include "guiQt/config.hpp"

#include <fwActivities/registry/Activities.hpp>
#include <fwActivities/registry/ActivityMsg.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/IAppConfigManager.hpp>

#include <fwTools/Failed.hpp>

#include <gui/view/IView.hpp>

#include <QObject>
#include <QPointer>

#include <map>
#include <set>


class QTabWidget;
namespace fwData
{
class Composite;
}
namespace guiQt
{
namespace editor
{

typedef const ::fwServices::IService::ConfigType ConfigType;

struct AppConfig
{
    typedef ::fwActivities::registry::ActivityAppConfigParam ParameterType;
    typedef std::vector< ParameterType > ParametersType;

    AppConfig()
    {
    }
    AppConfig(const ConfigType &config);

    std::string id;
    std::string title;
    std::string tabInfo;
    bool closable;
    ParametersType parameters;
};


/**
 * @brief   This editor manages tabs containing sub-configurations.
 *
 * It receives signals with ::fwActivities::registry::ActivityMsg containing the view information (config Id,
 * parameters, ...).
 *
 * It is usually used with the ::activities::action::SActivityLauncher action. This action sends the signals containing
 * the view information.
 *
 * In our applications, we mostly use the Series selector as main configuration. The main configuration is launched in
 * the first tab and is not closable.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service type="::gui::view::IView" impl="::guiQt::editor::DynamicView" autoConnect="yes" >
       <config dynamicConfigStartStop="false">
           <appConfig id="Visu2DID" title="Visu2D" >
               <parameters>
                   <parameter replace="SERIESDB" by="medicalData"  />
                   <parameter replace="IMAGE" by="@values.image"  />
               </parameters>
           </appConfig>
       </config>
   </service>
   @endcode
 * - \b appConfig: information needed to launch the main sub-configuration. This configuration is launched in the
 *      first tab, it can not be closed by the user.
 *   - \b id: config identifier
 *   - \b title: title of the created tab
 *   - \b parameters: list of the parameters needed to launch the configuration.
 *     - \b replace: name of the parameter as defined in the AppConfig
 *     - \b by: defines the string that will replace the parameter name. It should be a simple string (ex. frontal)
 *       or define a sesh@ path (ex. \@values.myImage). The root object of the sesh@ path is this service object.
 *
 * @section Slot Slot
 * - \b createTab( ::fwActivities::registry::ActivityMsg ): This slot allows to create a tab with the given activity
 *   information.
 *
 * @deprecated
 * This service will be replaced by the ::guiQt::editor::SDynamicView.
 * The usage is similar, the main difference is that the SDinamicView service only manage Activities.
 * In the configuration, you must replace the 'appConfig' tag by a 'mainActivity' tag, and create a simple Activity that
 * launchs your appConfig.
 *
 */
class GUIQT_CLASS_API DynamicView : public QObject,
                                    public ::gui::view::IView
{
Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro ( (DynamicView)(::gui::view::IView) );

    /// Constructor. Do nothing.
    GUIQT_API DynamicView() noexcept;

    /// Destructor. Do nothing.
    GUIQT_API virtual ~DynamicView() noexcept;

protected:

    /**
     * @brief Install the container.
     */
    virtual void starting();

    /**
     * @brief Destroy the container.
     */
    virtual void stopping();

    /**
     * @brief Update
     *
     */
    virtual void updating();

    /**
     * @brief Swap
     *
     * Update the editors.
     */
    virtual void swapping();

    /**
     * @brief Configure the view
     * @see fwGui::IGuiContainerSrv::initialize()
     */
    virtual void configuring();

    virtual void info( std::ostream &_sstream );


    typedef std::map< std::string, std::string> ReplaceMapType;
    struct DynamicViewInfo
    {
        ::fwGuiQt::container::QtContainer::sptr container;
        ::fwServices::IAppConfigManager::sptr helper;
        std::string wid;
        std::string title;
        bool closable;
        std::string icon;
        std::string tooltip;
        std::string tabID;
        std::string viewConfigID;
        ReplaceMapType replaceMap;
    };

    typedef std::map< QWidget*, DynamicViewInfo > DynamicViewInfoMapType;

    /**
     * @brief Launch tab
     */
    void launchTab(DynamicViewInfo& info);

    /// Slot: launch a new tab according to the receiving msg
    void createTab(::fwActivities::registry::ActivityMsg info);

    /**
     * @brief Build a DynamicViewInfo from an AppConfig
     */
    DynamicViewInfo buildDynamicViewInfo(const AppConfig& appConfig);

    QPointer<QTabWidget> m_tabWidget;
    QPointer<QWidget> m_currentWidget;

    std::map< std::string, unsigned int > m_titleToCount;
    std::set< std::string > m_tabIDList;

    DynamicViewInfoMapType m_dynamicInfoMap;
    bool m_dynamicConfigStartStop;

    AppConfig m_appConfig;

protected Q_SLOTS:

    void closeTab( int index, bool forceClose );

    void closeTabSignal(int index);

    void changedTab(int index);

};

}//namespace editor
} // guiQt

#endif /*__GUIQT_EDITOR_DYNAMICVIEW_HPP__*/
