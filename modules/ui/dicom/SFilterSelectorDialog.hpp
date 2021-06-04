/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "modules/ui/dicom/config.hpp"

#include <ui/base/IDialogEditor.hpp>

namespace sight::module::ui::dicom
{

/**
 * @brief   Display a dialog to select a dicom filter.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::ui::dicom::SFilterSelectorDialog">
       <inout key="filter" uid="..." />
       <selection mode="include" />
       <addSelection filter="sight::filter::dicom::custom::NoFilter" />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b filter [sight::data::String]: string that will contain the identifier of the choosen filter.
 * @subsection Configuration Configuration
 * None of these parameters are mandatory.
 *  - \b selection
 *      - \b mode (mandatory) : must be include (to add the selection to selector list ) or exclude (to exclude the
 * selection of the selector list).
 *  - \b addSelection
 *      - \b filter (mandatory) : Name of the filter to include/exclude to the choice list of the selector.
 */
class MODULE_UI_DICOM_CLASS_API SFilterSelectorDialog : public sight::ui::base::IDialogEditor
{
public:

    SIGHT_DECLARE_SERVICE(SFilterSelectorDialog, sight::ui::base::IDialogEditor);

    /**
     * @brief   Constructor. Do nothing (Just initialize parameters).
     *
     * By default, the SFilterSelectorDialog::m_filtersAreExcluded is set to true.
     */
    MODULE_UI_DICOM_API SFilterSelectorDialog();

    /// Destructor. Do nothing.
    MODULE_UI_DICOM_API virtual ~SFilterSelectorDialog() noexcept;

protected:

    ///Starts the service. Do nothing.
    MODULE_UI_DICOM_API void starting() override;

    /// Stops the service. Do nothing.
    MODULE_UI_DICOM_API void stopping() override;

    /**
     * @brief   This method initializes class member parameters from configuration elements.
     *
     * The method verifies if the configuration is well written and retrieves user parameter values.
     * Thanks to this method, SFilterSelectorDialog::m_selectedFilters value is up to date (cleared before
     * reconfiguring).
     **/
    MODULE_UI_DICOM_API void configuring() override;

    /// Create a dialog box to provide the user different available filters for the FilterSelector associated objects.
    /// Then, the selected filter is applied.
    MODULE_UI_DICOM_API void updating() override;

    /// Gives the name of the class. Do nothing.
    MODULE_UI_DICOM_API void info(std::ostream& _sstream) override;

private:

    /// Configure if selected filters are included or excluded.
    bool m_filtersAreExcluded;

    /**
     * @brief List of filters to be included or excluded.
     *
     * @see SFilterSelectorDialog::m_filtersAreExcluded.
     */
    std::vector<std::string> m_selectedFilters;
};

} // namespace sight::module::ui::dicom
