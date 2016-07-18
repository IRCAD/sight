/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOCALIBRATION_ACTION_SCALIBRATIONCONFIGURATION_HPP__
#define __VIDEOCALIBRATION_ACTION_SCALIBRATIONCONFIGURATION_HPP__

#include "videoCalibration/config.hpp"

#include <fwGui/IActionSrv.hpp>

#include <QObject>
#include <QPointer>
#include <QSpinBox>

namespace videoCalibration
{

namespace action
{

/**
 * @brief   This action shows a dialog to configure chessboard size.
 *
 * @section Signals Signals
 * - \b updatedChessboardSize(unsigned int, unsigned int, float) : Emitted when the parameters are updated.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::videoCalibration::SCalibrationConfiguration">
            <in key="parameters" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b parameters [::fwData::Composite]: composite containing the chessboard parameters.
 */
class VIDEOCALIBRATION_CLASS_API SCalibrationConfiguration : public QObject,
                                                             public ::fwGui::IActionSrv
{
Q_OBJECT
public:

    fwCoreServiceClassDefinitionsMacro ( (SCalibrationConfiguration)(::fwGui::IActionSrv) );

    /**
     * @name Signals
     * @{
     */

    /// Type of signal when objects are added
    typedef ::fwCom::Signal< void (unsigned int, unsigned int, float) > updatedChessboardSizeSignalType;
    VIDEOCALIBRATION_API static const ::fwCom::Signals::SignalKeyType s_UPDATED_CHESSBOARD_SIZE_SIG;

    /**
     * @}
     */

    /// Constructor. Do nothing.
    VIDEOCALIBRATION_API SCalibrationConfiguration() throw();

    /// Destructor. Do nothing.
    VIDEOCALIBRATION_API virtual ~SCalibrationConfiguration() throw();

protected:

    /// Start the action. Gets the aurora preference composite
    virtual void starting() throw(::fwTools::Failed);

    /// Does nothing
    virtual void stopping() throw(::fwTools::Failed);

    /// Shows a dialog to configure aurora port id and fps.
    virtual void updating() throw(::fwTools::Failed);

    /// Does nothing
    virtual void swapping() throw(::fwTools::Failed);

    /// Configure the service
    virtual void configuring() throw(fwTools::Failed);

    /// Overrides
    virtual void info( std::ostream &_sstream );

private:

    QPointer<QSpinBox > m_chessboardWidthEditor;
    QPointer<QSpinBox > m_chessboardHeightEditor;
    QPointer<QDoubleSpinBox > m_squareSizeChessboardEditor;

    updatedChessboardSizeSignalType::sptr m_sigUpdatedChessboardSize; ///< Update the chessboard size

};

} // namespace action
} // namespace videoCalibration

#endif /*__VIDEOCALIBRATION_ACTION_SCALIBRATIONCONFIGURATION_HPP__*/


