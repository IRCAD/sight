/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UINAVIGATION_SNAVIGATIONEDITOR_HPP__
#define __UINAVIGATION_SNAVIGATIONEDITOR_HPP__

#include <string>

#include <QObject>
#include <QPointer>

#include <fwTools/Failed.hpp>

#include <fwData/Object.hpp>

#include <fwThread/Timer.hpp>

#include <fwCom/Signals.hpp>

#include <gui/editor/IEditor.hpp>

#include "uiNavigation/config.hpp"

class QPushButton;
class QSlider;
class QLineEdit;

namespace uiNavigation
{

/**
 * @brief   SNavigationEditor is an editor to navigate in a scene.
 * @class   SNavigationEditor
 */
class UINAVIGATION_CLASS_API SNavigationEditor : public QObject,
                                                 public ::gui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro((SNavigationEditor)(::gui::editor::IEditor));

    /// Constructor.
    UINAVIGATION_API SNavigationEditor() noexcept;

    /// Destructor.
    UINAVIGATION_API virtual ~SNavigationEditor() noexcept;

    /**
     * @name Signal key and type for slider changes.
     * @{ */
    UINAVIGATION_API static const ::fwCom::Signals::SignalKeyType s_SLIDER_PROGRESSED_SIG;
    typedef ::fwCom::Signal < void (double) > SliderProgressedSignalType;
    /**  @} */

protected:

    /**
     * @name Overrides.
     * @{ */
    virtual void starting();
    virtual void stopping();
    virtual void updating();
    /**  @} */

    /**
     * @brief Overrides IService::configuring().
     * Configuration example :
       @code{.xml}
       <config>
        <sliderLength value="..." />     <!-- Length of slider showing progression in navigation (default = 100) -->
        <timerDuration value="..." />    <!-- Timer duration between iterations in milliseconds (default = 50)   -->
        <iterationsNumber value="..." /> <!-- Number of iterations within slider length (default = 100)          -->
       </config>
       @endcode
     *
     * @throw fwTools::Failed
     */
    virtual void configuring();

    /// Function called by the timer.
    void callBackTimer();

    /// Signal emitted when the slider moves.
    SliderProgressedSignalType::sptr m_sigSliderProgressed;

protected Q_SLOTS:

    // Called when the slider value changes.
    void onChangeValue(int newValue);

    // Called when the "Play" button is clicked.
    void onClickPlaySlotType();

    // Called when the "Pause" button is clicked.
    void onClickPauseSlotType();

    // Called when the "Reset" button is clicked.
    void onClickResetSlotType();

private:

    /// Current slider position showing progress in navigation.
    double m_sliderPosition;

    /// Slider length.
    int m_sliderLength;

    /// Number of steps in navigation.
    int m_iterationsNumber;

    /// Timer duration
    int m_timerDuration;

    /// Starts virtual navigation.
    QPointer< QPushButton > m_playButton;

    /// Pauses the navigation.
    QPointer< QPushButton > m_pauseButton;

    /// Resets the navigation.
    QPointer< QPushButton > m_resetButton;

    /// Slider showing progress in navigation.
    QPointer< QSlider > m_slider;

    /// Text showing progress in navigation.
    QPointer< QLineEdit > m_progressText;

    /// Timer controlling navigation.
    ::fwThread::Timer::sptr m_timer;

};

} // uiNavigation

#endif /* __UINAVIGATION_SNAVIGATIONEDITOR_HPP__ */

