/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIGENERICQT_ACTION_SHOWHELPCONTENTS_HPP__
#define __UIGENERICQT_ACTION_SHOWHELPCONTENTS_HPP__

#include <boost/filesystem/path.hpp>

#include <fwGui/IActionSrv.hpp>

#include "uiGenericQt/config.hpp"

namespace uiGeneric
{
namespace action
{

/**
 * @brief   This action show the help contents.
 * @class   ShowHelpContents
 */
class UIGENERICQT_CLASS_API ShowHelpContents : public ::fwGui::IActionSrv
{
public:
    fwCoreServiceClassDefinitionsMacro ( (ShowHelpContents)( ::fwGui::IActionSrv) );
    UIGENERICQT_API ShowHelpContents() throw();

    UIGENERICQT_API virtual ~ShowHelpContents() throw();

protected:

    /**
     * @brief Configuring method.
     *
     * XML configuration sample:
     * @code{.xml}
       <service impl="::uiGeneric::ui::action::ShowHelpContents" type="::fwGui::IActionSrv">
        <filename id=".../doc.qhc"/>
       </service>
       @endcode
     * This method is used to configure the service.
     */
    void configuring() throw (::fwTools::Failed);

    /// Starts action
    void starting() throw (::fwTools::Failed);

    /// Show the frame
    void updating() throw(::fwTools::Failed);

    /// Stops action
    void stopping() throw (::fwTools::Failed);

    UIGENERICQT_API void info(std::ostream &_sstream );

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

} // namespace uiGeneric

#endif /*__UIGENERICQT_ACTION_SHOWHELPCONTENTS_HPP__*/
