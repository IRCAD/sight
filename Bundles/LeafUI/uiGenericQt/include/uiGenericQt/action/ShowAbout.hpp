/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIGENERICQT_UI_ACTION_SHOWABOUT_HPP_
#define _UIGENERICQT_UI_ACTION_SHOWABOUT_HPP_

#include <QObject>
#include <QUrl>
#include <boost/filesystem/path.hpp>

#include <fwGui/IActionSrv.hpp>

#include "uiGenericQt/config.hpp"

namespace uiGeneric
{

namespace action
{

/**
 * @brief   This action show the about frame.
 * @class   ShowAbout.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 */
class UIGENERICQT_CLASS_API ShowAbout : public QObject, public ::fwGui::IActionSrv
{
    Q_OBJECT
public:
    fwCoreServiceClassDefinitionsMacro ( (ShowAbout)( ::fwGui::IActionSrv) ) ;
    UIGENERICQT_API ShowAbout() throw() ;

    UIGENERICQT_API virtual ~ShowAbout() throw() ;

protected:
    /**
    * @brief Configuring method.
    *
    * XML configuration sample:
    * @verbatim
    <service implementation="::uiGeneric::ui::action::ShowAbout" type="::fwGui::IActionSrv">
        <filename id=".../aboutFile.html"/>
    </service>
    @endverbatim
    * This method is used to configure the service.
    */
    void configuring() throw( ::fwTools::Failed ) ;

    /// Starts action
    void starting() throw (::fwTools::Failed);

    /// Does nothing
    void updating( ::boost::shared_ptr< const fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed);

    /// Show the frame
    void updating() throw(::fwTools::Failed);

    /// Stops action
    void stopping() throw (::fwTools::Failed);

    void info(std::ostream &_sstream ) ;

protected slots:

    void onUrlClicked(const QUrl & url );

private:
    /**
    * @brief the m_bServiceIsConfigured value is \b true
    * if the about file path is known.
    */
    bool m_bServiceIsConfigured;

    /**
    * @brief about file path.
    */
    ::boost::filesystem::path m_fsAboutPath;
};

} // namespace action

} // namespace uiGeneric

#endif /*UIGENERICQT_ACTION_SHOWABOUT_HPP_*/
