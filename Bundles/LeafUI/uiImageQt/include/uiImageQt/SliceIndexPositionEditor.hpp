/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIIMAGEQT_SLICEINDEXPOSITIONEDITOR_HPP_
#define _UIIMAGEQT_SLICEINDEXPOSITIONEDITOR_HPP_

#include <fwTools/Failed.hpp>
#include <gui/editor/IEditor.hpp>

#include <fwComEd/helper/MedicalImageAdaptor.hpp>
#include <fwWX/SliceSelector.hpp>

#include "uiImageQt/config.hpp"

namespace uiImage
{

/**
 * @brief   SliceIndexPositionEditor service.
 * @class   SliceIndexPositionEditor.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 */
class UIIMAGEQT_CLASS_API SliceIndexPositionEditor : public ::gui::editor::IEditor, public ::fwComEd::helper::MedicalImageAdaptor
{

public :


    fwCoreServiceClassDefinitionsMacro ( (SliceIndexPositionEditor)(::gui::editor::IEditor) ) ;

    /// Constructor. Do nothing.
    UIIMAGEQT_API SliceIndexPositionEditor() throw() ;

    /// Destructor. Do nothing.
    UIIMAGEQT_API virtual ~SliceIndexPositionEditor() throw() ;

protected:

    /// @brief The slice type: axial, frontal, sagittal.
    using ::fwComEd::helper::MedicalImageAdaptor::Orientation ;

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
    UIIMAGEQT_API virtual void info( std::ostream &_sstream ) ;

    UIIMAGEQT_API void updateSliceIndex();

    UIIMAGEQT_API void updateSliceType(Orientation type );

    UIIMAGEQT_API void sliceIndexNotification(unsigned int index);

    UIIMAGEQT_API void sliceTypeNotification( int type );

private:

    /// @brief The field IDs for the slice index.
    static const std::string* SLICE_INDEX_FIELDID[ 3 ];

    ::fwWX::SliceSelector* m_sliceSelectorPanel;

};

} // uiImage

#endif /*_UIIMAGEQT_SLICEINDEXPOSITIONEDITOR_HPP_*/


