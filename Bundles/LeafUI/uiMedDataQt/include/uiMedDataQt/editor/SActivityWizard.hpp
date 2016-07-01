/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIMEDDATAQT_EDITOR_SACTIVITYWIZARD_HPP__
#define __UIMEDDATAQT_EDITOR_SACTIVITYWIZARD_HPP__

#include "uiMedDataQt/config.hpp"
#include "uiMedDataQt/widget/ActivityDataView.hpp"

#include <fwActivities/registry/Activities.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwData/Vector.hpp>

#include <fwMedData/ActivitySeries.hpp>
#include <fwMedData/SeriesDB.hpp>

#include <gui/editor/IEditor.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include <QPointer>
#include <QPushButton>
#include <QLabel>

namespace uiMedData
{
namespace editor
{

/**
 * @brief This editor allows to select the data required by an activity in order to create the ActivitySeries.
 *
 * This editor displays a tab widget (one tab by data).
 * It works on a ::fwMedData::SeriesDB and adds the created activity series into the seriesDB.
 *
 * @section Slots Slots
 * - \b createActivity(std::string) : This slot displays the gui allowing to create a ::fwMedData::ActivitySeries with
 *      the required data for the given activity.
 * - \b updateActivity(::fwMedData::ActivitySeries::sptr) : This slot displays the gui allowing to update the required
 *      data for the given activity.
 * - \b updateActivitySeries(::fwMedData::Series::sptr) : This slot receives a ::fwMedData::Series, but works only if
 *   the series is an ActivitySeries, else it does nothing. It displays the gui allowing to update the required data for
 *   the given activity.
 *
 * @section Signals Signals
 * - \b activityCreated(::fwMedData::ActivitySeries::sptr) : This signal is emitted when the activitySeries is built.
 * - \b activityUpdated(::fwMedData::ActivitySeries::sptr) : This signal is emitted when the activitySeries is updated.
 * - \b canceled() : This signal is emitted when the cancel button is clicked.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service impl="::uiMedData::editor::SActivityWizard">
     <ioSelectorConfig>config</ioSelectorConfig>
     <activitySeries>seriesUid</activitySeries>
     <confirm>true</confirm>
     <cancel>true</cancel>
     <icons>
         <icon type="::fwMedData::ImageSeries" icon="@BUNDLE_PREFIX@/media_0-1/icons/ImageSeries.svg" />
         <icon type="::fwMedData::ModelSeries" icon="@BUNDLE_PREFIX@/media_0-1/icons/ModelSeries.svg" />
     </icons>
   </service>
   @endcode
 * - \b ioSelectorConfig : configuration for the SIOSelector service used to import data in this editor.
 * - \b icons : defines the icons displayed for a type of data
 *    - \b type : type of data
 *    - \b icon : path of the icon to display
 * - \b activitySeries (optional) : uid of the activity series to update. It is only used when the update() method is
 *      called.
 * - \b confirm (optional, true by default): if true, the editor proposes a confirmation dialog when the activity is
 *   updated.
 * - \b cancel (optional, true by default): if true, the editor proposes a cancel button. On cancel click, the editor is
 *   emptied and the signal 'canceled' is emitted.

 */
class UIMEDDATAQT_CLASS_API SActivityWizard : public QObject,
                                              public ::gui::editor::IEditor
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SActivityWizard)(::gui::editor::IEditor) );

    /// Constructor. Do nothing.
    UIMEDDATAQT_API SActivityWizard() throw();

    /// Destructor. Do nothing.
    UIMEDDATAQT_API virtual ~SActivityWizard() throw();

    /**
     * @name Slot API
     * @{
     */
    UIMEDDATAQT_API static const ::fwCom::Slots::SlotKeyType s_CREATE_ACTIVITY_SLOT;
    UIMEDDATAQT_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_ACTIVITY_SLOT;
    UIMEDDATAQT_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_ACTIVITY_SERIES_SLOT;
    /// @}

    /**
     * @name Signal API
     * @{
     */
    UIMEDDATAQT_API static const ::fwCom::Signals::SignalKeyType s_ACTIVITY_CREATED_SIG;
    typedef ::fwCom::Signal< void ( ::fwMedData::ActivitySeries::sptr ) > ActivityCreatedSignalType;

    UIMEDDATAQT_API static const ::fwCom::Signals::SignalKeyType s_ACTIVITY_UPDATED_SIG;
    typedef ::fwCom::Signal< void ( ::fwMedData::ActivitySeries::sptr ) > ActivityUpdatedSignalType;

    UIMEDDATAQT_API static const ::fwCom::Signals::SignalKeyType s_CANCELED_SIG;
    typedef ::fwCom::Signal< void () > CanceledSignalType;
    /// @}

protected:

    ///This method creates the editor gui.
    virtual void starting() throw(::fwTools::Failed);

    ///This method destroys the editor gui.
    virtual void stopping() throw(::fwTools::Failed);

    /// Update the activity if it is defined in the configuration, else does nothing.
    virtual void updating() throw(::fwTools::Failed);

    /// Initialize the editor.
    virtual void configuring() throw(fwTools::Failed);

private:


    enum class Mode : std::uint8_t
    {
        CREATE,
        UPDATE
    };

    /// SLOT: Displays the gui allowing to select the required data for the activity.
    void createActivity(std::string activityID);

    /// SLOT: Displays the gui allowing to update the required data for the activity.
    void updateActivity(::fwMedData::ActivitySeries::sptr activitySeries);

    /// SLOT: Displays the gui allowing to update the required data for the activity.
    /**
     * @brief SLOT: It displays the gui allowing to update the required data for the given activity.
     * @note This slot receives a ::fwMedData::Series, but works only if the series is an ActivitySeries, else it does
     *       nothing.
     */
    void updateActivitySeries(::fwMedData::Series::sptr series);

    /**
     * @brief Called when the user click on the 'apply' Button.
     *
     * if mode == CREATE : It creates the activitySeries and add it to the seriesDB.
     * else, it update the current activity series.
     */
    void onBuildActivity();

    /// Called when the tab selection changed. It check if the current data are properly selected.
    void onTabChanged(int index);

    /// Called when the user click on the 'reset' Button.
    void onReset();

    /// Called when the user click on the 'cancel' Button.
    void onCancel();

    ::fwMedData::ActivitySeries::sptr m_actSeries; ///< Activity series builded

    QPointer<widget::ActivityDataView> m_activityDataView; ///< view used to select required data for activity
    QPointer<QPushButton> m_okButton; ///<  Button 'Apply' or 'Next' to validate the activity creation
    QPointer<QPushButton> m_resetButton; ///< Button to clear the activity parameters
    QPointer<QPushButton> m_cancelButton; ///< Button to cancel the activity creation
    QPointer<QLabel> m_title; ///< Label to show activity title
    QPointer<QLabel> m_description; ///< Label to show activity description

    std::string m_ioSelectorConfig; ///< configuration used to import data

    widget::ActivityDataView::ObjectIconMapType m_objectIcons; ///< Map defining the icon associated to an object.

    Mode m_mode; ///< editor mode (CREATE or UPDATE)

    bool m_confirmUpdate; ///< if true, the editor proposes a confirmation dialog when the activity is updated.
    bool m_isCancelable;  /// true if the cancel button is proposed


    ActivityCreatedSignalType::sptr m_sigActivityCreated; ///< Signal emitted when the activitySeries is created
    ActivityCreatedSignalType::sptr m_sigActivityUpdated; ///< Signal emitted when the activitySeries is updated
    CanceledSignalType::sptr m_sigCanceled;               /// Signal emitted when the creation is canceled.

};

} //editor
} // uiMedData


#endif // __UIMEDDATAQT_EDITOR_SACTIVITYWIZARD_HPP__
