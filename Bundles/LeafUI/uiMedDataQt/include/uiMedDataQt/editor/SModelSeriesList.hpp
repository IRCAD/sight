/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
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

class QListWidget;
class QCheckBox;
class QListWidgetItem;

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

    virtual void configuring() throw(fwTools::Failed);

    /// Overrides
    virtual void info( std::ostream &_sstream ) ;

    typedef std::map< std::string, SPTR(::fwData::Reconstruction) > OrganNameReconstruction;

    void updateReconstructions();

protected Q_SLOTS:

    /// Slot called when new current item is setted in m_organChoice
    void onCurrentItemChanged ( QListWidgetItem * current, QListWidgetItem * previous );

    void onShowReconstructions(int state);

    void onOrganChoiceVisibility(QListWidgetItem * item);

private:

    void refreshVisibility();
    QPointer< QCheckBox > m_showCheckBox;
    QPointer< QListWidget > m_organChoice;
    OrganNameReconstruction m_map ;

};

} // namespace editor
} // namespace uiMedData

#endif /*_UIMEDDATAQT_EDITOR_SMODELSERIESLIST_HPP__*/


