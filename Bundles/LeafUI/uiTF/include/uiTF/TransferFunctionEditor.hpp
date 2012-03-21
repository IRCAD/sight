/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UITF_TRANSFERFUNCTIONEDITOR_HPP_
#define _UITF_TRANSFERFUNCTIONEDITOR_HPP_

#include <QObject>

#include <fwData/Composite.hpp>
#include <fwData/TransfertFunction.hpp>

#include <gui/editor/IEditor.hpp>

#include "export.hpp"

class QComboBox;
class QPushButton;
class QIcon;

namespace uiTF
{

/**
 * @brief   TransferFunctionEditor service.
 * @class   TransferFunctionEditor.
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class UITF_CLASS_API TransferFunctionEditor : public QObject, public ::gui::editor::IEditor
{
    Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro ( (TransferFunctionEditor)(::gui::editor::IEditor) ) ;

    /// Basic constructor, do nothing.
    UITF_API TransferFunctionEditor();

    /// Basic destructor, do nothing.
    UITF_API virtual ~TransferFunctionEditor() throw ();

protected:

    /**
    * @brief Configuring the transfer function editor.
    *
    * Example of configuration
    * @verbatim
    <service uid="GENERIC_UID_tfm" type="::gui::editor::IEditor" implementation="::uiTF::TransferFunctionEditor" autoComChannel="yes" >
        <config selectedTFKey="SelectedTF" tfPoolFwID="TFSelections" />
    </service>
    @endverbatim
    * - <image id="myImage" /> : Set the link between the service and the associated image.
    *
    * \b id : mandatory (no default value) : set the id of the associated image.
    */
    UITF_API virtual void configuring() throw( ::fwTools::Failed );

    /// Start the TransferFunctionEditor, create Container, place in Buttons, ComboBox, Layout, and connect them.
    UITF_API virtual void starting() throw( ::fwTools::Failed );

    /// Update the TransferFunctionEditor, do nothing.
    UITF_API virtual void updating() throw( ::fwTools::Failed );

    /// Update the TransferFunctionEditor when message, do nothing.
    UITF_API virtual void updating(::fwServices::ObjectMsg::csptr _msg) throw( ::fwTools::Failed );

    /// Stop the TransferFunctionEditor, disconnect Buttons and Combo Box, delete them and clean the container.
    UITF_API virtual void stopping() throw( ::fwTools::Failed );

    /// Initialize the transfer functions, get fields m_transfertFunctionCompositeId and m_transfertFunctionId associated to the related image, add their names to the ComboBox. If the image does not contain any TF, the create a few from the ressources of the Bundle.
    UITF_API void initTransferFunctions();

    /// Check if the image contain the specified TF.
    UITF_API bool hasTransferFunctionName(const std::string & _sName);

    /// Create a string that represents a TF name not already present in the image. For example, if blabla is already used, it will return blabla_1.
    UITF_API std::string createTransferFunctionName( const std::string & _sBasename );

    /// Update the image with the selected TF in the ComboBox.
    UITF_API void updateTransferFunction();

    /// Get the current transfer function pool
    UITF_API ::fwData::Composite::sptr getTFSelection() const;

    /// Get the current transfer function
    UITF_API ::fwData::TransfertFunction_VERSION_II::sptr getSelectedTransferFunction() const;

private slots:

    void deleteTF();
    void newTF();
    void reinitializeTFPool();
    void renameTF();
    void importTF();
    void exportTF();
    void presetChoice(int index);

private:

    QComboBox *m_pTransferFunctionPreset;
    QPushButton *m_deleteButton;
    QPushButton *m_newButton;
    QPushButton *m_reinitializeButton;
    QPushButton *m_renameButton;
    QPushButton *m_importButton;
    QPushButton *m_exportButton;
    QWidget* m_container;

    /**
     * @brief Identifier of the field containing the current selection of TransfertFunction.
     * by defaults use ::fwComEd::Dictionary::m_transfertFunctionId
     */
    std::string m_selectedTFKey;

    /// fwID of tf selection ( used during configuration )
    std::string m_tfSelectionFwID;
};

}
#endif
