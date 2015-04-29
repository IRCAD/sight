/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UITRACKING_SARUCOTRACKEREDITOR_HPP__
#define __UITRACKING_SARUCOTRACKEREDITOR_HPP__

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include <QLabel>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QCheckBox>

#include <fwData/Object.hpp>

#include <gui/editor/IEditor.hpp>

#include "uiTracking/config.hpp"

namespace uiTracking
{
/**
 * @brief   SArucoTrackerEditor service is used to handle parameters of aruco tracker.
 * @class   SArucoTrackerEditor
 *
 * Service registered details : \n
 * fwServicesRegisterMacro(::gui::editor::IEditor, ::SArucoTrackerEditor::SArucoTrackerEditor, ::fwData::Object)
 */
class UITRACKING_CLASS_API SArucoTrackerEditor :
    public QObject,
    public ::gui::editor::IEditor
{
Q_OBJECT;

public:
    fwCoreServiceClassDefinitionsMacro((SArucoTrackerEditor)(::gui::editor::IEditor));

    /**
     * @name Signals API
     * @{
     */
    typedef ::fwCom::Signal< void (unsigned int) > MethodChangedSignalType;
    typedef ::fwCom::Signal< void (unsigned int) > CornerMethodChangedSignalType;
    typedef ::fwCom::Signal< void (double) >       ConstantChangedSignalType;
    typedef ::fwCom::Signal< void (double) >       BlockSizeChangedSignalType;
    typedef ::fwCom::Signal< void (double) >       BorderWidthChangedSignalType;
    typedef ::fwCom::Signal< void (double) >       PatternWidthChangedSignalType;
    typedef ::fwCom::Signal< void (bool) >         TagsDisplayedSignalType;
    typedef ::fwCom::Signal< void (unsigned int) > SpeedChangedSignalType;

    /// Key in m_signals map of signal m_sigMethodChanged
    UITRACKING_API static const ::fwCom::Signals::SignalKeyType s_METHOD_CHANGED_SIG;

    /// Key in m_signals map of signal m_sigCornerMethodChanged
    UITRACKING_API static const ::fwCom::Signals::SignalKeyType s_CORNERMETHOD_CHANGED_SIG;

    /// Key in m_signals map of signal m_sigConstantChanged
    UITRACKING_API static const ::fwCom::Signals::SignalKeyType s_CONSTANT_CHANGED_SIG;

    /// Key in m_signals map of signal m_sigBlockSizeChanged
    UITRACKING_API static const ::fwCom::Signals::SignalKeyType s_BLOCKSIZE_CHANGED_SIG;

    /// Key in m_signals map of signal m_sigBorderWidthChanged
    UITRACKING_API static const ::fwCom::Signals::SignalKeyType s_BORDERWIDTH_CHANGED_SIG;

    /// Key in m_signals map of signal m_sigPatternWidthChanged
    UITRACKING_API static const ::fwCom::Signals::SignalKeyType s_PATTERNWIDTH_CHANGED_SIG;

    /// Key in m_signals map of signal m_sigDebugEnabled
    UITRACKING_API static const ::fwCom::Signals::SignalKeyType s_TAGS_DISPLAYED_SIG;

    /// Key in m_signals map of signal m_sigSpeedChanged
    UITRACKING_API static const ::fwCom::Signals::SignalKeyType s_SPEED_CHANGED_SIG;

    ///@}


    /**
     * @brief Constructor.
     */
    UITRACKING_API SArucoTrackerEditor() throw ();

    /**
     * @brief Destructor.
     */
    UITRACKING_API ~SArucoTrackerEditor() throw ();

    ///nothing
    UITRACKING_API void configuring() throw (fwTools::Failed);

    /**
     * @brief Starting method : This method is used to initialize the service.
     */
    UITRACKING_API void starting() throw (fwTools::Failed);

    /**
     * @brief Stopping method : This method is used to stop the service.
     */
    UITRACKING_API void stopping() throw (fwTools::Failed);

    /**
     * @brief Updating method : This method is used to update the service.
     */
    UITRACKING_API void updating() throw (fwTools::Failed);

protected:

    /**
     * @brief Add a new point acquisition.
     * @param[in] capture Composite containing the point acquisitions from the two cameras.
     */
    UITRACKING_API void update();

protected Q_SLOTS:

    /// Slot called when Method comboBox value changed.
    void onMethodChanged(int value);

    /// Slot called when Method comboBox value changed.
    void onConstantChanged(double value);

    /// Slot called when Method comboBox value changed.
    void onBlockSizeChanged(double value);

    /// Slot called when Method comboBox value changed.
    void onBorderWidthChanged(double value);

    /// Slot called when Method comboBox value changed.
    void onPatternWidthChanged(double value);

    /// Slot called when Method comboBox value changed.
    void onCornerMethodChanged(int value);

    ///Slot called when debug checkbox is enabled/disabled
    void onTagsDisplayed(int value);

    ///Slot called when speed comboBox changed
    void onSpeedChanged(int value);
private:

    QComboBox*      m_methodBox; ///< method box
    QComboBox*      m_cornerBox; ///< corner box
    QComboBox*      m_speedBox; ///< speed box
    QDoubleSpinBox* m_constantSB; ///< constant spin box
    QDoubleSpinBox* m_blockSizeSB; ///< block size spin box
    QDoubleSpinBox* m_borderWidthSB; ///< border width spin box
    QDoubleSpinBox* m_patternWidthSB; ///< pattern width spin box
    QCheckBox*      m_displayTags; ///< check box to displaying

    MethodChangedSignalType::sptr m_sigMethodChanged; ///< signal method changed
    CornerMethodChangedSignalType::sptr m_sigCornerMethodChanged; ///< signal corner method changed
    ConstantChangedSignalType::sptr m_sigConstantChanged; ///< signal constant changed
    BlockSizeChangedSignalType::sptr m_sigBlockSizeChanged; ///< signal block size changed
    BorderWidthChangedSignalType::sptr m_sigBorderWidthChanged; ///< signal border width changed
    PatternWidthChangedSignalType::sptr m_sigPatternWidthChanged; ///< signal pattern width changed
    TagsDisplayedSignalType::sptr m_sigTagsDisplayed; ///< signal to display tag
    SpeedChangedSignalType::sptr m_sigSpeedChanged; ///< signal for speed changing

};
} // namespace uiTracking

#endif // __UITRACKING_SARUCOTRACKEREDITOR_HPP__
