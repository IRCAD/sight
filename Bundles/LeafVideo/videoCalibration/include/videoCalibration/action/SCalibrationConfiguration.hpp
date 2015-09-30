/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOCALIBRATION_ACTION_SCALIBRATIONCONFIGURATION_HPP__
#define __VIDEOCALIBRATION_ACTION_SCALIBRATIONCONFIGURATION_HPP__

#include "videoCalibration/config.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include <fwData/Composite.hpp>

#include <fwGui/IActionSrv.hpp>

#include <fwTools/Failed.hpp>

#include <QObject>
#include <QPointer>
#include <QSpinBox>

namespace fwData
{
class Composite;
}

namespace videoCalibration
{

namespace action
{

/**
 * @brief   This action shows a dialog to configure chessboard size.
 * @class   SCalibrationConfiguration
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
    typedef ::fwCom::Signal< void (int, int, float) > updatedChessboardSizeSignalType;
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

    /**
     * @brief Configures the service
     *
     * @verbatim
         <service uid="..." impl="::videoCalibration::action::SCalibrationConfiguration" autoConnect="no" >
             <config>
                 <parametersKey>chessboardParameters</parametersKey>
             </config>
         </service>
       @endverbatim
     * - \b parametersKey : key of composite containing the chessboard parameter.
     */

    virtual void configuring() throw(fwTools::Failed);

    /// Overrides
    virtual void info( std::ostream &_sstream );

private:

    QPointer<QSpinBox > m_chessboardWidthEditor;
    QPointer<QSpinBox > m_chessboardHeightEditor;
    QPointer<QDoubleSpinBox > m_squareSizeChessboardEditor;

    std::string m_compKey;

    ::fwData::Composite::sptr m_paramComp;

    updatedChessboardSizeSignalType::sptr m_sigUpdatedChessboardSize; ///< Update the chessboard size

};

} // namespace action
} // namespace videoCalibration

#endif /*__VIDEOCALIBRATION_ACTION_SCALIBRATIONCONFIGURATION_HPP__*/


