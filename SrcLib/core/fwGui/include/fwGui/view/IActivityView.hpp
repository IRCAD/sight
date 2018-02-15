/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwGui/config.hpp"
#include "fwGui/IGuiContainerSrv.hpp"

#include <fwActivities/registry/Activities.hpp>

#include <fwCom/Slots.hpp>

#include <fwMedData/ActivitySeries.hpp>
#include <fwMedData/Series.hpp>

namespace fwGui
{
namespace view
{

/**
 * @brief Defines the base class for services displaying activity view
 *
 * @section Slots Slots
 * - \b launchActivity( ::fwMedData::ActivitySeries::sptr ): This slot allows to create a view for the given activity
 *   series.
 * - \b launchActivitySeries( ::fwMedData::Series::sptr ): This slot allows to create a view for the given activity
 *   series.
 *
 *  * @section XML XML Configuration
 * @code{.xml}
   <service type="::guiQt::editor::SDynamicView" autoConnect="yes" >
     <mainActivity id="SDBActivity" />
     <parameters>
         <parameter replace="SERIESDB" by="medicalData"  />
         <parameter replace="ICON_PATH" by="media-0.1/icons/app.ico"  />
     </parameters>
   </service>
   @endcode
 * - \b mainActivity (optional): information about the main activity (first tab). The activity series will be generated.
 *   This activity must not have requirement.
 *   - \b id : identifier of the activity
 * - \b parameters (optional) : additional parameters used to launch the activities
 *    - \b parameter: defines a parameter
 *        - \b replace: name of the parameter as defined in the AppConfig
 *        - \b by: defines the string that will replace the parameter name. It should be a simple string (ex.
 *          frontal) or define a camp path (ex. \@values.myImage). The root object of the sesh@ path if the
 *          composite contained in the ActivitySeries.
 */
class FWGUI_CLASS_API IActivityView : public ::fwGui::IGuiContainerSrv
{
public:

    fwCoreNonInstanciableClassDefinitionsMacro( (IActivityView)(::fwGui::IGuiContainerSrv) )

    FWGUI_API static const ::fwCom::Slots::SlotKeyType s_LAUNCH_ACTIVITY_SLOT;
    FWGUI_API static const ::fwCom::Slots::SlotKeyType s_LAUNCH_ACTIVITY_SERIES_SLOT;

protected:

    typedef ::fwActivities::registry::ActivityAppConfigParam ParameterType;
    typedef ::fwActivities::registry::ActivityAppConfig::ActivityAppConfigParamsType ParametersType;
    typedef std::map< std::string, std::string> ReplaceMapType;

    /// Constructor. Do nothing.
    FWGUI_API IActivityView();

    /// Destructor. Do nothing.
    FWGUI_API virtual ~IActivityView();

    /// Parse the configuration
    FWGUI_API virtual void configuring() override;

    /**
     * @brief Slot: Launch the given activity in a new tab.
     * @note The same activity series cannot be launch in two different tabs.
     */
    FWGUI_API virtual void launchActivity(::fwMedData::ActivitySeries::sptr activitySeries) = 0;

    /**
     * @brief Slot: Launch the given activity in a new tab.
     * @note The same activity series cannot be launch in two different tabs.
     */
    FWGUI_API virtual void launchActivitySeries(::fwMedData::Series::sptr series);

    /**
     * @brief Check if the activity is valid by calling the activity validator.
     * @return Return true if the given activity is valid
     */
    FWGUI_API virtual bool validateActivity(::fwMedData::ActivitySeries::sptr activitySeries) const;

    /// Create the activity series given in 'mainActivity' configuration
    FWGUI_API virtual ::fwMedData::ActivitySeries::sptr createMainActivity() const;

    /**
     * @brief Translate parameters from source object.
     *
     * Replace camp path (ex.@values.myParam.image) by the uid of the corresponding object.
     *
     * @param[in] sourceObj source object used to find sub-object form camp path
     * @param[in] parameters list of parameters to translate
     * @param[out] replaceMap map containing the translated parameter
     */
    FWGUI_API virtual void translateParameters( ::fwData::Object::sptr sourceObj, const ParametersType& parameters,
                                                ReplaceMapType& replaceMap );

    std::string m_mainActivityId; ///< configuration id of the main activity

    ParametersType m_parameters; ///< parameters given in configuration
};

} // namespace view
} // namespace fwGui
