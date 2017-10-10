/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UITF_TRANSFERFUNCTIONEDITOR_HPP__
#define __UITF_TRANSFERFUNCTIONEDITOR_HPP__

#include "uiTF/config.hpp"

#include <fwData/Composite.hpp>
#include <fwData/TransferFunction.hpp>

#include <gui/editor/IEditor.hpp>

#include <boost/filesystem/path.hpp>

#include <QObject>

class QComboBox;
class QPushButton;
class QIcon;

namespace uiTF
{

/**
 * @brief Editor to select a transfer function.
 *
 * The available transfer function are stored in a composite (transferFunctionPool), if it contains at most one
 * transfert function, this service will read transfer function from the given pathes.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::uiTF::TransferFunctionEditor">
       <in key="currentTF" uid="..." optional="yes" />
       <inout key="tfPool" uid="..." />
       <out key="tf" uid="..." />
       <config useDefaultPath="yes">
           <path>....</path>
           <path>....</path>
           <path>....</path>
       </config>
   </service>
   @endcode
 *
 * @subsection Input Input
 * - \b currentTF [::fwData::TransferFunction](optional) : current transfer function used to change editor selection. It
 *      should be the same TF as the output.
 *      If it is not set, the default GreyLevel will be selected at start and the editor will not listen the change of
 *      TF in another service. Don't forget to set 'optional="yes"' when you use this input, otherwise the service will
 *      not start if a TF is not previously defined.
 * @subsection In-Out In-Out
 * - \b tfPool [::fwData::Composite]: composite containing the transfer function.
 * @subsection Output Output
 * - \b tf [::fwData::TransferFunction]: selected transfer function.
 * @subsection Configuration Configuration
 * - \b useDefaultPath (optional)(default = yes): if true, load tf files from uiTF bundle.
 * - \b path (optional): path to a directory containing tf files.
 */
class UITF_CLASS_API TransferFunctionEditor : public QObject,
                                              public ::gui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro( (TransferFunctionEditor)(::gui::editor::IEditor) );

    /// Basic constructor, do nothing.
    UITF_API TransferFunctionEditor();

    /// Basic destructor, do nothing.
    UITF_API virtual ~TransferFunctionEditor() noexcept;

protected:

    /// Configure the transfer function editor.
    UITF_API virtual void configuring() override;

    /// Start the TransferFunctionEditor, create Container, place in Buttons, ComboBox, Layout, and connect them.
    UITF_API virtual void starting() override;

    /// Update the TransferFunctionEditor, do nothing.
    UITF_API virtual void updating() override;

    /// Stop the TransferFunctionEditor, disconnect Buttons and Combo Box, delete them and clean the container.
    UITF_API virtual void stopping() override;

    /// Selects the current transfer function
    UITF_API void swapping(const KeyType& key);

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Composite::s_ADDED_OBJECTS_SIG to this::s_UPDATE_SLOT
     * Connect Composite::s_CHANGED_OBJECTS_SIG to this::s_UPDATE_SLOT
     * Connect Composite::s_REMOVED_OBJECTS_SIG to this::s_UPDATE_SLOT
     */
    UITF_API virtual KeyConnectionsMap getAutoConnections() const;

    /**
     * @brief Initialize the transfer functions.
     *
     * Add their names to the ComboBox. If the composite does not contain any TF (or only the default grey level TF,
     * the service creates a few from the ressources of the Bundle.
     */
    UITF_API void initTransferFunctions();

    /// Check if the image contain the specified TF.
    UITF_API bool hasTransferFunctionName(const std::string& _sName) const;

    /// Create a string that represents a TF name not already present in the composite. For example, if blabla is
    /// already used, it will return blabla_1.
    UITF_API std::string createTransferFunctionName( const std::string& _sBasename ) const;

    /// Update the output transferFunction with the selected TF in the ComboBox.
    UITF_API void updateTransferFunction();

    /// Update the TF preset from the TF pool
    UITF_API void updateTransferFunctionPreset();

private Q_SLOTS:

    void deleteTF();
    void newTF();
    void reinitializeTFPool();
    void renameTF();
    void importTF();
    void exportTF();
    void presetChoice(int index);

private:

    typedef std::vector< ::boost::filesystem::path > PathContainerType;

    QComboBox* m_pTransferFunctionPreset;
    QPushButton* m_deleteButton;
    QPushButton* m_newButton;
    QPushButton* m_reinitializeButton;
    QPushButton* m_renameButton;
    QPushButton* m_importButton;
    QPushButton* m_exportButton;

    ::fwData::TransferFunction::sptr m_selectedTF;

    /// Paths of the tf files to load.
    PathContainerType m_paths;
};

}
#endif // __UITF_TRANSFERFUNCTIONEDITOR_HPP__
