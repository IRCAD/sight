/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IODICOMEXT_DCMTK_EDITOR_SFILTERSELECTORDIALOG_HPP__
#define __IODICOMEXT_DCMTK_EDITOR_SFILTERSELECTORDIALOG_HPP__

#include <gui/editor/IDialogEditor.hpp>

#include "ioDicomExt/config.hpp"

namespace ioDicomExt
{

namespace dcmtk
{

namespace editor
{

/**
 * @brief   Defines the service interface managing the editor service for object.
 * @class   SFilterSelectorDialog
 *
 * @date    2014.
 */
class IODICOMEXT_CLASS_API SFilterSelectorDialog : public ::gui::editor::IDialogEditor
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SFilterSelectorDialog)( ::gui::editor::IDialogEditor) );

    /**
     * @brief   Constructor. Do nothing (Just initialize parameters).
     *
     * By default, the SFilterSelectorDialog::m_filtersAreExcluded is set to true.
     */
    IODICOMEXT_API SFilterSelectorDialog();

    /// Destructor. Do nothing.
    IODICOMEXT_API virtual ~SFilterSelectorDialog() throw();

protected:

    ///Starts the service. Do nothing.
    IODICOMEXT_API void starting() throw( ::fwTools::Failed );

    /// Stops the service. Do nothing.
    IODICOMEXT_API void stopping() throw( ::fwTools::Failed );

    /**
     * @brief   This method initializes class member parameters from configuration elements.
     *
     * The method verifies if the configuration is well written and retrieves user parameter values.
     * Thanks to this method, SFilterSelectorDialog::m_selectedFilters value is up to date (cleared before reconfiguring).
     *
     * Sample of configuration :
     * @verbatim
          <service uid="GENERIC_UID_filter" type="::gui::editor::IDialogEditor" impl="::ioDicomExt::dcmtk::editor::SFilterSelectorDialog" autoConnect="no">
              <selection mode="include" />
              <addSelection filter="::fwDicomIOFilter::custom::NoFilter" />
          </service>
     * @endverbatim
     * With :
     * None of these parameters are mandatory.
     *  - \b selection
     *      - \b mode (mandatory) : must be include (to add the selection to selector list ) or exclude (to exclude the selection of the selector list).
     *  - \b addSelection
     *      - \b filter (mandatory) : Name of the filter to include/exclude to the choice list of the selector.
     **/
    IODICOMEXT_API void configuring() throw( ::fwTools::Failed );

    /// Create a dialog box to provide the user different available filters for the FilterSelector associated objects. Then, the selected filter is applied.
    IODICOMEXT_API void updating() throw( ::fwTools::Failed );

    /// Gives the name of the class. Do nothing.
    IODICOMEXT_API void info( std::ostream &_sstream );

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

} // namespace editor
} // namespace dcmtk
} // namespace ioDicomExt

#endif // __IODICOMEXT_DCMTK_EDITOR_SFILTERSELECTORDIALOG_HPP__

