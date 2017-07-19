/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIVISUQT_TRANSFORMATIONMATRIXEDITOR_HPP__
#define __UIVISUQT_TRANSFORMATIONMATRIXEDITOR_HPP__

#include <QObject>
#include <QPointer>
#include <QSlider>

#include <fwTools/Failed.hpp>

#include <gui/editor/IEditor.hpp>

#include "uiVisuQt/config.hpp"

namespace uiVisu
{

/**
 * @brief   TransformationMatrixEditor service is represented by a slider. It update the rotation matrix from the slider angle value.
 * @class   TransformationMatrixEditor
 *
 * @date    2010.
 */
class UIVISUQT_CLASS_API TransformationMatrixEditor : public QObject,
                                                      public ::gui::editor::IEditor
{

Q_OBJECT
public:

    fwCoreServiceClassDefinitionsMacro ( (TransformationMatrixEditor)(::gui::editor::IEditor) );

    /// Constructor. Do nothing.
    UIVISUQT_API TransformationMatrixEditor() noexcept;

    /// Destructor. Do nothing.
    UIVISUQT_API virtual ~TransformationMatrixEditor() noexcept;

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    /// Install the layout.
    virtual void starting();

    /// Destroy the layout.
    virtual void stopping();

    /// Do nothing
    virtual void updating();

    /// Do nothing
    virtual void swapping();

    /// Configure the editor. Do nothing.
    virtual void configuring();

    /// Overrides
    virtual void info( std::ostream &_sstream );

protected Q_SLOTS:
    /// Called when the slider value change.
    void onSliderChange(int value);

private:
    QPointer< QSlider > m_angleSlider;


};

} // uiVisu

#endif /*__UIVISUQT_TRANSFORMATIONMATRIXEDITOR_HPP__*/


