/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIGENERICQT_ACTION_SSHOWABOUT_HPP__
#define __UIGENERICQT_ACTION_SSHOWABOUT_HPP__

#include "uiGenericQt/config.hpp"

#include <fwGui/IActionSrv.hpp>

#include <boost/filesystem/path.hpp>

#include <QObject>
#include <QSize>
#include <QUrl>

namespace uiGenericQt
{

namespace action
{

/**
 * @brief   This action show the about frame.
 */
class UIGENERICQT_CLASS_API SShowAbout : public QObject,
                                         public ::fwGui::IActionSrv
{

Q_OBJECT
public:
    fwCoreServiceClassDefinitionsMacro( (SShowAbout)( ::fwGui::IActionSrv) );

    /**
     * @name Constructor/Destructor
     * @{ */
    UIGENERICQT_API SShowAbout() noexcept;
    UIGENERICQT_API virtual ~SShowAbout() noexcept;
    /**  @} */

protected:
    /**
     * @brief Configuring method.
     *
     * XML configuration sample:
       @code{.xml}
       <service impl="::uiGenericQt::action::SShowAbout" type="::fwGui::IActionSrv">
         <filename id=".../aboutFile.html"/>
         <title>title</title>
         <size width="200" height="200" />
       </service>
       @endcode
     * This method is used to configure the service.
     */
    void configuring() override;

    /// Starts action
    void starting() override;

    /// Shows the frame
    void updating() override;

    /// Stops action
    void stopping() override;

    /// Prints service info
    void info(std::ostream& _sstream) override;

protected Q_SLOTS:

    /**
     * @brief Triggered when an URL is clicked in the about frame.
     *
     * @param url clicked URL
     */
    void onUrlClicked(const QUrl& url);

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

} // namespace uiGenericQt

#endif // __UIGENERICQT_ACTION_SSHOWABOUT_HPP__

