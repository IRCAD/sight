/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIGENERICQT_ACTION_SHOWHELPCONTENTS_HPP__
#define __UIGENERICQT_ACTION_SHOWHELPCONTENTS_HPP__

#include "uiGenericQt/config.hpp"

#include <fwGui/IActionSrv.hpp>

#include <boost/filesystem/path.hpp>

namespace uiGenericQt
{
namespace action
{

/**
 * @brief   This action show the help contents.
 */
class UIGENERICQT_CLASS_API ShowHelpContents : public ::fwGui::IActionSrv
{
public:
    fwCoreServiceClassDefinitionsMacro( (ShowHelpContents)( ::fwGui::IActionSrv) );
    UIGENERICQT_API ShowHelpContents() noexcept;

    UIGENERICQT_API virtual ~ShowHelpContents() noexcept;

protected:

    /**
     * @brief Configuring method.
     *
     * XML configuration sample:
     * @code{.xml}
       <service impl="::uiGenericQt::ui::action::ShowHelpContents" type="::fwGui::IActionSrv">
        <filename id=".../doc.qhc"/>
       </service>
       @endcode
     * This method is used to configure the service.
     */
    void configuring() override;

    /// Starts action
    void starting() override;

    /// Show the frame
    void updating() override;

    /// Stops action
    void stopping() override;

    UIGENERICQT_API void info(std::ostream& _sstream ) override;

private:
    /**
     * @brief the m_bServiceIsConfigured value is \b true
     * if the help files path is known.
     */
    bool m_bServiceIsConfigured;

    /**
     * @brief help files path.
     */
    ::boost::filesystem::path m_fsHelpPath;
};

} // namespace action

} // namespace uiGenericQt

#endif /*__UIGENERICQT_ACTION_SHOWHELPCONTENTS_HPP__*/
