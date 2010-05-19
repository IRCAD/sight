/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
/* ***** BEGIN CONTRIBUTORS BLOCK *****
 * Initial authors:
 *  - Jean-Baptiste.Fasquel (LISA Laboratory, Angers University, France)
 * ****** END CONTRIBUTORS BLOCK ****** */

#ifndef _UIIMAGE_OPACITYEDITOR_HPP_
#define _UIIMAGE_OPACITYEDITOR_HPP_

#include <fwTools/Failed.hpp>
#include <gui/editor/IEditor.hpp>

#include "uiImage/config.hpp"

namespace uiImage
{

class Slider;

/**
 * @brief   OpacityEditor service.
 * @class   OpacityEditor.
 */
class UIIMAGE_CLASS_API OpacityEditor : public ::gui::editor::IEditor
{

public :


    fwCoreServiceClassDefinitionsMacro ( (OpacityEditor)(::gui::editor::IEditor) ) ;

    /// Constructor. Do nothing.
    UIIMAGE_API OpacityEditor() throw() ;

    /// Destructor. Do nothing.
    UIIMAGE_API virtual ~OpacityEditor() throw() ;

protected:

    ///This method launches the IEditor::starting method.
    virtual void starting() throw(::fwTools::Failed);

    ///This method launches the IEditor::stopping method.
    virtual void stopping() throw(::fwTools::Failed);

    /// Managment of observations ( overides )
    virtual void updating( ::boost::shared_ptr< const fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed);

    virtual void updating() throw(::fwTools::Failed);

    virtual void swapping() throw(::fwTools::Failed);

    virtual void configuring() throw(fwTools::Failed);

    /// Overrides
    UIIMAGE_API virtual void info( std::ostream &_sstream ) ;

    /// Updates the slider according to image opacity changes
    UIIMAGE_API void updateOpacity();

    /// Updates image opacity according to interactions on the slider
    UIIMAGE_API void sliceOpacityNotification(unsigned int index);

private:

    ::uiImage::Slider* m_sliceSelectorPanel;
    float m_opacity;

};

} // uiImage

#endif /*_UIIMAGE_OPACITYEDITOR_HPP_*/


