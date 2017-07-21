/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIVISUQT_TRANSFORMATIONMATRIXEDITOR_HPP__
#define __UIVISUQT_TRANSFORMATIONMATRIXEDITOR_HPP__

#include "uiVisuQt/config.hpp"

#include <fwTools/Failed.hpp>

#include <gui/editor/IEditor.hpp>

#include <QObject>
#include <QPointer>
#include <QSlider>

namespace uiVisu
{

/**
 * @brief   TransformationMatrixEditor service is represented by a slider. It update the rotation matrix from the slider
 * angle value.
 *
 * @code{.xml}
        <service uid="..." type="::uiVisu::TransformationMatrixEditor" >
            <inout key="matrix" uid="..." />
        </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b output [::fwData::TransformationMatrix3D]: Output matrix.
 */

class UIVISUQT_CLASS_API TransformationMatrixEditor : public QObject,
                                                      public ::gui::editor::IEditor
{

Q_OBJECT
public:

    fwCoreServiceClassDefinitionsMacro( (TransformationMatrixEditor)(::gui::editor::IEditor) );

    /**
     * @brief Constructor.
     */
    UIVISUQT_API TransformationMatrixEditor() noexcept;

    /**
     * @brief Destructor.
     */
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
    virtual void info( std::ostream& _sstream );

protected Q_SLOTS:
    /// Called when the slider value change.
    void onSliderChange(int value);

private:
    QPointer< QSlider > m_angleSlider;

};

} //namespace uiVisu

#endif // __UIVISUQT_TRANSFORMATIONMATRIXEDITOR_HPP__

