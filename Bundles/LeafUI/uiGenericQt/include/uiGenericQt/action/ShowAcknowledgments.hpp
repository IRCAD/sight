/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIGENERICQT_ACTION_SHOWACKNOWLEDGMENTS_HPP__
#define __UIGENERICQT_ACTION_SHOWACKNOWLEDGMENTS_HPP__

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
 * @brief   This action show the acknowlegments frame.
 * @class   ShowAcknowledgments
 */
class UIGENERICQT_CLASS_API ShowAcknowledgments : public QObject,
                                                  public ::fwGui::IActionSrv
{
Q_OBJECT
public:
    fwCoreServiceClassDefinitionsMacro ( (ShowAcknowledgments)( ::fwGui::IActionSrv) );
    UIGENERICQT_API ShowAcknowledgments() throw();

    UIGENERICQT_API virtual ~ShowAcknowledgments() throw();

protected:

    /**
     * @brief configure the action.
     * @code{.xml}
       <service type="::fwGui::IActionSrv" impl="::uiGeneric::action::ShowAcknowledgments" autoConnect="no">
        <filename id=".../ack.html" />
       </service>
       @endcode
     */
    UIGENERICQT_API void configuring() throw( ::fwTools::Failed );

    /// Starts action
    void starting() throw (::fwTools::Failed);

    /// Show the frame
    void updating() throw (::fwTools::Failed);

    /// Stops action
    void stopping() throw (::fwTools::Failed);

    UIGENERICQT_API void info(std::ostream &_sstream );

protected Q_SLOTS:

    void onUrlClicked(const QUrl & url );

private:
    /**
     * @brief the m_bServiceIsConfigured value is \b true
     * if the help files path is known.
     */
    bool m_bServiceIsConfigured;

    /**
     * @brief acknowledge file path.
     */
    ::boost::filesystem::path m_fsAckPath;
};

} // namespace action
} // namespace uiGeneric

#endif /*__UIGENERICQT_ACTION_SHOWACKNOWLEDGMENTS_HPP__*/
