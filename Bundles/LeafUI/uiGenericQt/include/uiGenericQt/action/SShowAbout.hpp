/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIGENERICQT_ACTION_SSHOWABOUT_HPP__
#define __UIGENERICQT_ACTION_SSHOWABOUT_HPP__

#include <QObject>
#include <QSize>
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
 * @class   SShowAbout
 */
class UIGENERICQT_CLASS_API SShowAbout : public QObject,
                                         public ::fwGui::IActionSrv
{

Q_OBJECT
public:
    fwCoreServiceClassDefinitionsMacro ( (SShowAbout)( ::fwGui::IActionSrv) );

    /**
     * @name Constructor/Destructor
     * @{ */
    UIGENERICQT_API SShowAbout() throw();
    UIGENERICQT_API virtual ~SShowAbout() throw();
    /**  @} */

protected:
    /**
     * @brief Configuring method.
     *
     * XML configuration sample:
       @code{.xml}
       <service impl="::uiGeneric::action::SShowAbout" type="::fwGui::IActionSrv">
         <filename id=".../aboutFile.html"/>
         <title>title</title>
         <size width="200" height="200" />
       </service>
       @endcode
     * This method is used to configure the service.
     */
    void configuring() throw( ::fwTools::Failed );

    /// Starts action
    void starting() throw (::fwTools::Failed);

    /// Shows the frame
    void updating() throw(::fwTools::Failed);

    /// Stops action
    void stopping() throw (::fwTools::Failed);

    /// Prints service info
    void info(std::ostream &_sstream);

protected Q_SLOTS:

    /**
     * @brief Triggered when an URL is clicked in the about frame.
     *
     * @param url clicked URL
     */
    void onUrlClicked(const QUrl & url);

private:
    /// Set to 'true' if the about file path is known.
    bool m_bServiceIsConfigured;

    /**
     * @brief about file path.
     */
    ::boost::filesystem::path m_fsAboutPath;

    /// Frame title (default value set to "About").
    std::string m_title;

    /// Frame size.
    QSize m_size;
};

} // namespace action

} // namespace uiGeneric

#endif // __UIGENERICQT_ACTION_SSHOWABOUT_HPP__

