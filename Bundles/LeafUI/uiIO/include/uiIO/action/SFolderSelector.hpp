/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIIO_ACTION_SFOLDERSELECTOR_HPP__
#define __UIIO_ACTION_SFOLDERSELECTOR_HPP__

#include "uiIO/config.hpp"

#include <fwGui/IActionSrv.hpp>

#include <boost/filesystem/path.hpp>

namespace uiIO
{
namespace action
{

/**
 * @brief   Raise a pop-up dialog to select a directory, then send it via a signal.
 *
 * @section Signals Signals
 * - \b folderSelector(::boost::filesystem::path) : Emitted when the directory is chosen.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::uiIO::action::SFolderSelector" >
            <dialogTitle>Select a folder</dialogTitle>
        </service>
   @endcode
 * @subsection Configuration Configuration:
 * - \b dialogTitle (optional): title of the dialog.
 */
class UIIO_CLASS_API SFolderSelector : public ::fwGui::IActionSrv
{
public:
    fwCoreServiceClassDefinitionsMacro( (SFolderSelector)( ::fwGui::IActionSrv) )

    /**
     * @name Signals API
     * @{
     */
    typedef ::fwCom::Signal< void ( ::boost::filesystem::path ) > FolderSelectedSignalType;
    static const ::fwCom::Signals::SignalKeyType s_FOLDER_SELECTED_SIG;
    /// @}

    UIIO_API SFolderSelector() noexcept;
    UIIO_API virtual ~SFolderSelector() noexcept;

protected:

    /// Do nothing
    UIIO_API void configuring() override;

    /// Do nothing.
    UIIO_API void starting() override;

    /// Show the dialog and send the signal with the chosen directory
    UIIO_API void updating() override;

    /// Do nothing.
    UIIO_API void stopping() override;

private:

    /// Optional title for the dialog
    std::string m_dialogTitle;
};

} // namespace action
} // namespace uiIO

#endif // __UIIO_ACTION_SFOLDERSELECTOR_HPP__
