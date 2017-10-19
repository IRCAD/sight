/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IODICOM_SFILTERSELECTORDIALOG_HPP__
#define __IODICOM_SFILTERSELECTORDIALOG_HPP__

#include "ioDicom/config.hpp"

#include <gui/editor/IDialogEditor.hpp>

namespace ioDicom
{

/**
 * @brief   Defines the service interface managing the editor service for object.
 */
class IODICOM_CLASS_API SFilterSelectorDialog : public ::gui::editor::IDialogEditor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SFilterSelectorDialog)( ::gui::editor::IDialogEditor) );

    /**
     * @brief   Constructor. Do nothing (Just initialize parameters).
     *
     * By default, the SFilterSelectorDialog::m_filtersAreExcluded is set to true.
     */
    IODICOM_API SFilterSelectorDialog();

    /// Destructor. Do nothing.
    IODICOM_API virtual ~SFilterSelectorDialog() noexcept;

protected:

    ///Starts the service. Do nothing.
    IODICOM_API void starting() override;

    /// Stops the service. Do nothing.
    IODICOM_API void stopping() override;

    /**
     * @brief   This method initializes class member parameters from configuration elements.
     *
     * The method verifies if the configuration is well written and retrieves user parameter values.
     * Thanks to this method, SFilterSelectorDialog::m_selectedFilters value is up to date (cleared before
     * reconfiguring).
     *
     * Sample of configuration :
     * @code{.xml}
          <service uid="GENERIC_UID_filter" type="::gui::editor::IDialogEditor"
     * impl="::ioDicom::dcmtk::editor::SFilterSelectorDialog" autoConnect="no">
              <selection mode="include" />
              <addSelection filter="::fwDicomIOFilter::custom::NoFilter" />
          </service>
     * @endcode
     * With :
     * None of these parameters are mandatory.
     *  - \b selection
     *      - \b mode (mandatory) : must be include (to add the selection to selector list ) or exclude (to exclude the
     * selection of the selector list).
     *  - \b addSelection
     *      - \b filter (mandatory) : Name of the filter to include/exclude to the choice list of the selector.
     **/
    IODICOM_API void configuring() override;

    /// Create a dialog box to provide the user different available filters for the FilterSelector associated objects.
    /// Then, the selected filter is applied.
    IODICOM_API void updating() override;

    /// Gives the name of the class. Do nothing.
    IODICOM_API void info( std::ostream& _sstream ) override;

private:

    /// Configure if selected filters are included or excluded.
    bool m_filtersAreExcluded;

    /**
     * @brief List of filters to be included or excluded.
     *
     * @see SFilterSelectorDialog::m_filtersAreExcluded.
     */
    std::vector< std::string > m_selectedFilters;
};

} // namespace ioDicom

#endif // __IODICOM_SFILTERSELECTORDIALOG_HPP__

