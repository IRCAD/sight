/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOQT_ACTION_SVIDEOCONFIGURATION_HPP__
#define __VIDEOQT_ACTION_SVIDEOCONFIGURATION_HPP__

#include <QObject>
#include <QPointer>
#include <QLineEdit>

#include <fwTools/Failed.hpp>

#include <fwGui/IActionSrv.hpp>

#include "videoQt/config.hpp"

namespace fwData
{
class Composite;
}

namespace videoQt
{

namespace action
{

/**
 * @brief   This action shows a dialog to configure video video directory.
 * @class   SVideoConfiguration
 * @note    This service works with preferences and does nothing if the preferences service is not found.
 */
class VIDEOQT_CLASS_API SVideoConfiguration : public QObject,
                                              public ::fwGui::IActionSrv
{
Q_OBJECT
public:

    fwCoreServiceClassDefinitionsMacro ( (SVideoConfiguration)(::fwGui::IActionSrv) );

    /// Constructor. Do nothing.
    VIDEOQT_API SVideoConfiguration() throw();

    /// Destructor. Do nothing.
    VIDEOQT_API virtual ~SVideoConfiguration() throw();

    typedef std::vector< std::string > DevicesVectType;
protected:

    /// Start the action. Gets the aurora preference composite
    virtual void starting() throw(::fwTools::Failed);

    /// Does nothing
    virtual void stopping() throw(::fwTools::Failed);

    /// Shows a dialog to configure aurora port id and fps.
    virtual void updating() throw(::fwTools::Failed);

    /// Does nothing
    virtual void swapping() throw(::fwTools::Failed);

    /// Does nothing
    virtual void configuring() throw(fwTools::Failed);

    /// Overrides
    virtual void info( std::ostream &_sstream );

protected Q_SLOTS:
    void onSelectVideoDir();

private:

    QPointer<QLineEdit> m_videoDirEdit;

    SPTR(::fwData::Composite) m_videoPref;
};

} // namespace action
} // namespace videoQt

#endif /*__VIDEOQT_ACTION_SVIDEOCONFIGURATION_HPP__*/


