/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOQT_EDITOR_SCAMERA_HPP__
#define __VIDEOQT_EDITOR_SCAMERA_HPP__

#include <QObject>
#include <QComboBox>
#include <QPointer>

#include <fwTools/Failed.hpp>

#include <gui/editor/IEditor.hpp>

#include "videoQt/config.hpp"

namespace videoQt
{
namespace editor
{

/**
 * @brief   This editor allows to select the device to use. It updates the data camera identifier.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="..." type="::videoQt::editor::SCamera" autoConnect="no">
        <inout key="camera" uid="..."/>
        <videoSupport>yes</videoSupport>
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b camera [::arData::Camera]: device used
 *
 * @subsection Configuration Configuration
 * - \b videoSupport (optional, by default "no") : if we can open a video file in addition with cameras
 *
 */
class VIDEOQT_CLASS_API SCamera : public QObject,
                                  public ::gui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro ( (SCamera)(::gui::editor::IEditor) );

    /// Constructor. Do nothing.
    VIDEOQT_API SCamera() throw();

    /// Destructor. Do nothing.
    VIDEOQT_API virtual ~SCamera() throw();

protected:


    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    /// Installs the layout
    virtual void starting() throw(::fwTools::Failed);

    /// Destroys the layout
    virtual void stopping() throw(::fwTools::Failed);

    /// Does nothing
    virtual void updating() throw(::fwTools::Failed);

    /// Does nothing
    virtual void swapping() throw(::fwTools::Failed);

    /**
     * @brief Configure the service
     *
     * @code{.xml}
        <service uid="..." type="::gui::editor::IEditor" impl="::videoQt::editor::SCamera" autoConnect="no">
            <videoSupport>...</videoSupport>
        </service>
       @endcode
     * - \b videoSupport (optional, by default "no") : if we can open a video file in addition with cameras
     */
    virtual void configuring() throw(fwTools::Failed);

protected Q_SLOTS:

    /// Calls when user select another device
    void onApply(int index);

private:

    void onChooseFile();
    void onChooseStream();
    void onChooseDevice();

    /// Combobox for camera selection
    QPointer<QComboBox> m_devicesComboBox;

    /// Do we offer the possibility to select a video file (no by default) ?
    bool m_bVideoSupport;
};

} // editor
} // videoQt

#endif /*__VIDEOQT_EDITOR_SCAMERA_HPP__*/


