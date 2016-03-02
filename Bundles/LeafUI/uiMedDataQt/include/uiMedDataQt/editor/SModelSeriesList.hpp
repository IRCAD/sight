/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIMEDDATAQT_EDITOR_SMODELSERIESLIST_HPP__
#define __UIMEDDATAQT_EDITOR_SMODELSERIESLIST_HPP__

#include "uiMedDataQt/config.hpp"

#include <gui/editor/IEditor.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwData/Reconstruction.hpp>

#include <fwTools/Failed.hpp>


#include <QPointer>
#include <QObject>
#include <QPushButton>


class QTreeWidget;
class QCheckBox;
class QListWidgetItem;
class QTreeWidgetItem;

namespace uiMedData
{
namespace editor
{


class ValueView;

/**
 * @brief   SModelSeriesList service.
 * @class   SModelSeriesList
 * @date    2013.
 */
class UIMEDDATAQT_CLASS_API SModelSeriesList :  public QObject,
                                                public ::gui::editor::IEditor
{
Q_OBJECT
public:

    fwCoreServiceClassDefinitionsMacro ( (SModelSeriesList)(::gui::editor::IEditor) );

    /// Constructor. Do nothing.
    UIMEDDATAQT_API SModelSeriesList() throw();

    /// Destructor. Do nothing.
    UIMEDDATAQT_API virtual ~SModelSeriesList() throw();

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect ModelSeries::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect ModelSeries::s_RECONSTRUCTIONS_ADDED_SIG to this::s_UPDATE_SLOT
     * Connect ModelSeries::s_RECONSTRUCTIONS_REMOVED_SIG to this::s_UPDATE_SLOT
     */
    UIMEDDATAQT_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:

    ///This method launches the IEditor::starting method.
    virtual void starting() throw(::fwTools::Failed);

    ///This method launches the IEditor::stopping method.
    virtual void stopping() throw(::fwTools::Failed);

    virtual void updating() throw(::fwTools::Failed);

    virtual void swapping() throw(::fwTools::Failed);

    /**
     * @brief Configures the editor.
     *
     * Configuration example :
       @code{.xml}
       <enable_hide_all>true</enable_hide_all>
       <columns>
         <organ_name>@organ_name</organ_name>
         <volume_cc view="positive" >@volume</volume_cc>
       </columns>
       @endcode
     *
     * \b enable_hide_all : if 'true', allows to hide all models through a single checkbox displayed in UI (default
     * value is 'true', allowed values are 'true' and 'false').
     *
     * \b columns : defines colums to be shown in reconstruction list. XML child element names follow
     * ::fwData::Reconstruction serialization attribute names.
     * The attribute 'view' is optional and can has the following values :
     *  - positive : a numeric value is displayed only if it is positive. Otherwise, 'Unknown' is displayed.
     *
     * @throw fwTools::Failed
     */
    virtual void configuring() throw(fwTools::Failed);

    /// Overrides
    virtual void info( std::ostream &_sstream );

    typedef std::map< std::string, ValueView* > DisplayedInformation;

    void updateReconstructions();

    void fillTree();

    static const ::fwCom::Signals::SignalKeyType s_REC_DISPLAY_MODIFIED__SIG;
    typedef ::fwCom::Signal< void (bool) > RecDisplayModifiedSignalType;
    static const ::fwCom::Signals::SignalKeyType s_RECONSTRUCTION_SELECTED_SIG;
    typedef ::fwCom::Signal< void (::fwData::Object::sptr) > ReconstructionSelectedSignalType;
    static const ::fwCom::Signals::SignalKeyType s_EMPTIED_SELECTION_SIG;
    typedef ::fwCom::Signal< void () > EmptiedSelectionSignalType;
    static const ::fwCom::Slots::SlotKeyType s_SHOW_RECONSTRUCTIONS_SLOT;
    typedef ::fwCom::Slot< void (bool) > ShowReconstructionsSlotType;


protected Q_SLOTS:

    /// Slot called when new current item is setted in m_organChoice
    void onCurrentItemChanged ( QTreeWidgetItem * current, QTreeWidgetItem * previous );

    void onCurrentItemChanged ( QTreeWidgetItem * current, int column );

    void onShowReconstructions(int state);

    void onOrganChoiceVisibility(QTreeWidgetItem * item, int column);

    void onCheckAllCheckBox();
    void onUnCheckAllCheckBox();

private:

    /// SLOT: Show (or hide) reconstructions
    void showReconstructions(bool show);

    void refreshVisibility();

    void onCheckAllBoxes(bool visible);

    QPointer<QPushButton> m_checkAllButton;
    QPointer<QPushButton> m_unCheckAllButton;

    QPointer< QCheckBox > m_showCheckBox;
    QPointer< QTreeWidget > m_tree;
    DisplayedInformation m_displayedInfo;

    bool m_enableHideAll;

    /// Signal emitted when m_showCheckBox value changed
    RecDisplayModifiedSignalType::sptr m_sigRecDisplayModified;

    /// Signal emitted when a reconstruction is selected
    ReconstructionSelectedSignalType::sptr m_sigReconstructionSelected;

    /// Signal emitted when we clean the list
    EmptiedSelectionSignalType::sptr m_sigEmptiedSelection;

    /// Slot to show (or hide) reconstructions
    ShowReconstructionsSlotType::sptr m_slotShowReconstuctions;
};

} // namespace editor
} // namespace uiMedData

#endif /*__UIMEDDATAQT_EDITOR_SMODELSERIESLIST_HPP__*/


