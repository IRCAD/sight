/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIPATIENTDBQT_SPATIENTDBINSERTER_HPP_
#define _UIPATIENTDBQT_SPATIENTDBINSERTER_HPP_

#include <gui/editor/IDialogEditor.hpp>

#include "uiPatientDBQt/config.hpp"

namespace uiPatientDBQt
{

/**
 * @brief   A dialog editor to push an image from a selection in patientDB
 * @class   SPatientDBInserter.
 * @author  IRCAD (Research and Development Team).
 * @date    2012.
 */
class UIPATIENTDBQT_CLASS_API SPatientDBInserter : public ::gui::editor::IDialogEditor
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SPatientDBInserter)(::gui::editor::IDialogEditor) ) ;

    /**
     * @brief Overrides
     @verbatim
     <service ... >
        <image selectionUid="myComposite" key="myImage" />
     </service>
     @endverbatim
     *
     * Set the object uid of selection ( which is a composite ) and the key of
     * your image this composite.
     */
    UIPATIENTDBQT_API virtual void configuring() throw ( ::fwTools::Failed );

    /// Overrides, does nothing
    UIPATIENTDBQT_API virtual void starting() throw ( ::fwTools::Failed );

    /// Overrides, does nothing
    UIPATIENTDBQT_API virtual void stopping() throw ( ::fwTools::Failed );

    /// Create a patient from image selected and push it in the patientDB ( service associated object )
    UIPATIENTDBQT_API virtual void updating() throw ( ::fwTools::Failed );

    /// Overrides, does nothing
    UIPATIENTDBQT_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );

    /// Just call stopping/starting
    UIPATIENTDBQT_API virtual void swapping() throw ( ::fwTools::Failed );

protected :

    /// Constructor
    UIPATIENTDBQT_API SPatientDBInserter();

    /// Destructor
    UIPATIENTDBQT_API virtual ~SPatientDBInserter() throw();

private :

    /**
     * @brief Retrieves selected image, if we have not selected image or if the
     * buffer is null, the method returns null sptr
     */
    ::fwData::Image::sptr getImageFromSelection() const;

    /// Create A PDB from selected image
    ::fwData::PatientDB::sptr createPDB() const;

    /// Selection uid ( a composite )
    std::string m_selectionUid;

    /// Key representing the selected image stored in composite
    std::string m_selectionKey;
};

} // namespace uiPatientDBQt

#endif /*_UIPATIENTDBQT_SPATIENTDBINSERTER_HPP_*/

