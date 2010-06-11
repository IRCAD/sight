/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIIMAGE_SLICEINDEXPOSITIONEDITOR_HPP_
#define _UIIMAGE_SLICEINDEXPOSITIONEDITOR_HPP_

#include <fwTools/Failed.hpp>
#include <gui/editor/IEditor.hpp>

#include <fwComEd/helper/MedicalImageAdaptor.hpp>
#include <fwWX/SliceSelector.hpp>

#include "uiImage/config.hpp"

namespace uiImage
{

/**
 * @brief   SliceIndexPositionEditor service allows to change the slice index of an image.
 * @class   SliceIndexPositionEditor.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 *
 * This is represented by
 *  - a slider to select the slice index
 *  - a choice list to select the slice orientation (axial, frontal, sagittal)
 */
class UIIMAGE_CLASS_API SliceIndexPositionEditor : public ::gui::editor::IEditor, public ::fwComEd::helper::MedicalImageAdaptor
{

public :


    fwCoreServiceClassDefinitionsMacro ( (SliceIndexPositionEditor)(::gui::editor::IEditor) ) ;

    /// Constructor. Do nothing.
    UIIMAGE_API SliceIndexPositionEditor() throw() ;

    /// Destructor. Do nothing.
    UIIMAGE_API virtual ~SliceIndexPositionEditor() throw() ;

protected:

    /// @brief The slice type: axial, frontal, sagittal.
    using ::fwComEd::helper::MedicalImageAdaptor::Orientation ;

    /**
     * @brief Install the layout.
     *
     * This method launches the IEditor::starting method.
     */
    virtual void starting() throw(::fwTools::Failed);

    /**
     * @brief Destroy the layout.
     *
     * This method launches the IEditor::stopping method.
     */
    virtual void stopping() throw(::fwTools::Failed);

    /// Management of observations : update editor according to the received message
    virtual void updating( ::boost::shared_ptr< const fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed);

    /// Update editor information from the image
    virtual void updating() throw(::fwTools::Failed);

    /// Swap of image
    virtual void swapping() throw(::fwTools::Failed);

    /**
     * @brief Configure the editor.
     *
     * Example of configuration
     * @verbatim
     <service uid="slider_negato1" implementation="::uiImage::SliceIndexPositionEditor" type="::gui::editor::IEditor" autoComChannel="yes" sliceIndex="axial" />
       @endverbatim
       \b sliceIndex must be "axial", "frontal" or "sagittal".
     */
    virtual void configuring() throw(fwTools::Failed);

    /// Overrides
    UIIMAGE_API virtual void info( std::ostream &_sstream ) ;

    /// Update the editor slider from the image slice index.
    UIIMAGE_API void updateSliceIndex();

    /// Update the editor slice type choice from the image slice type.
    UIIMAGE_API void updateSliceType(Orientation type );

    /// This method is called when the slider is move. Notify the slice index is modified.
    UIIMAGE_API void sliceIndexNotification(unsigned int index);

    /// This method is called when the slice type selected change. Notify the slice type is modified.
    UIIMAGE_API void sliceTypeNotification( int type );

private:

    /// @brief The field IDs for the slice index.
    static const std::string* SLICE_INDEX_FIELDID[ 3 ];

    ::fwWX::SliceSelector* m_sliceSelectorPanel;

};

} // uiImage

#endif /*_UIIMAGE_SLICEINDEXPOSITIONEDITOR_HPP_*/


