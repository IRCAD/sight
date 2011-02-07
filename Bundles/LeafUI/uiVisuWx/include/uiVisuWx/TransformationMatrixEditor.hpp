/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIVISUWX_TRANSFORMATIONMATRIXEDITOR_HPP_
#define _UIVISUWX_TRANSFORMATIONMATRIXEDITOR_HPP_

#include <wx/slider.h>

#include <fwTools/Failed.hpp>

#include <gui/editor/IEditor.hpp>

#include "uiVisuWx/config.hpp"

namespace uiVisu
{

/**
 * @brief   TransformationMatrixEditor service is represented by a slider. It update the rotation matrix from the slider angle value.
 * @class   TransformationMatrixEditor.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 */
class UIVISUWX_CLASS_API TransformationMatrixEditor : public ::gui::editor::IEditor
{

public :

    fwCoreServiceClassDefinitionsMacro ( (TransformationMatrixEditor)(::gui::editor::IEditor) ) ;

    /// Constructor. Do nothing.
    UIVISUWX_API TransformationMatrixEditor() throw() ;

    /// Destructor. Do nothing.
    UIVISUWX_API virtual ~TransformationMatrixEditor() throw() ;

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    /// Install the layout.
    virtual void starting() throw(::fwTools::Failed);

    /// Destroy the layout.
    virtual void stopping() throw(::fwTools::Failed);

    /// Do nothing
    virtual void updating( ::boost::shared_ptr< const fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed);

    /// Do nothing
    virtual void updating() throw(::fwTools::Failed);

    /// Do nothing
    virtual void swapping() throw(::fwTools::Failed);

    /// Configure the editor. Do nothing.
    virtual void configuring() throw(fwTools::Failed);

    /// Overrides
    virtual void info( std::ostream &_sstream ) ;


    /// Called when the slider value change.
    void onSliderChange( wxCommandEvent& event );

private:

    wxSlider * m_angleSlider;

};

} // uiVisu

#endif /*_UIVISUWX_TRANSFORMATIONMATRIXEDITOR_HPP_*/


