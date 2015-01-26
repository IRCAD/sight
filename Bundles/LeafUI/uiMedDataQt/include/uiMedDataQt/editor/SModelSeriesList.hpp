/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIMEDDATAQT_EDITOR_SMODELSERIESLIST_HPP__
#define _UIMEDDATAQT_EDITOR_SMODELSERIESLIST_HPP__

#include <QPointer>
#include <QObject>

#include <fwTools/Failed.hpp>

#include <gui/editor/IEditor.hpp>

#include "uiMedDataQt/config.hpp"

class QTreeWidget;
class QCheckBox;
class QListWidgetItem;
class QTreeWidgetItem;

namespace fwData
{
    class Reconstruction;
}

namespace fwServices
{
    class ObjectMsg;
}

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
class UIMEDDATAQT_CLASS_API SModelSeriesList :  public QObject, public ::gui::editor::IEditor
{
    Q_OBJECT
public :

    fwCoreServiceClassDefinitionsMacro ( (SModelSeriesList)(::gui::editor::IEditor) ) ;

    /// Constructor. Do nothing.
    UIMEDDATAQT_API SModelSeriesList() throw() ;

    /// Destructor. Do nothing.
    UIMEDDATAQT_API virtual ~SModelSeriesList() throw() ;

protected:

    ///This method launches the IEditor::starting method.
    virtual void starting() throw(::fwTools::Failed);

    ///This method launches the IEditor::stopping method.
    virtual void stopping() throw(::fwTools::Failed);

    /// Managment of observations ( overides )
    virtual void receiving( SPTR(const ::fwServices::ObjectMsg) _msg ) throw(::fwTools::Failed);

    virtual void updating() throw(::fwTools::Failed);

    virtual void swapping() throw(::fwTools::Failed);

    /**
     * @brief Configures the editor.
     *
     * Configuration example :
     @verbatim
     <enable_hide_all>true</enable_hide_all>
     <columns>
         <organ_name>@organ_name</organ_name>
         <volume_cc view="positive" >@volume</volume_cc>
     </columns>
     @endverbatim
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
    virtual void info( std::ostream &_sstream ) ;

    typedef std::map< std::string, ValueView* > DisplayedInformation;

    void updateReconstructions();

    void fillTree();

protected Q_SLOTS:

    /// Slot called when new current item is setted in m_organChoice
    void onCurrentItemChanged ( QTreeWidgetItem * current, QTreeWidgetItem * previous );

    void onCurrentItemChanged ( QTreeWidgetItem * current, int column );

    void onShowReconstructions(int state);

    void onOrganChoiceVisibility(QTreeWidgetItem * item , int column);

private:

    void refreshVisibility();
    QPointer< QCheckBox > m_showCheckBox;
    QPointer< QTreeWidget > m_tree;
    DisplayedInformation m_displayedInfo ;

    bool m_enableHideAll;
};

} // namespace editor
} // namespace uiMedData

#endif /*_UIMEDDATAQT_EDITOR_SMODELSERIESLIST_HPP__*/


