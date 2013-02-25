/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIACQUISITIONQT_SORGANSERIESLISTEDITOR_HPP__
#define __UIACQUISITIONQT_SORGANSERIESLISTEDITOR_HPP__

#include <QPointer>
#include <QObject>

#include <fwTools/Failed.hpp>

#include <gui/editor/IEditor.hpp>

#include "uiAcquisitionQt/config.hpp"

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

namespace uiAcquisition
{

/**
 * @brief   SOrganSeriesListEditor service.
 * @class   SOrganSeriesListEditor.
 * @date    2013.
 */
class UIACQUISITIONQT_CLASS_API SOrganSeriesListEditor :  public QObject, public ::gui::editor::IEditor
{
    Q_OBJECT
public :

    fwCoreServiceClassDefinitionsMacro ( (SOrganSeriesListEditor)(::gui::editor::IEditor) ) ;

    /// Constructor. Do nothing.
    UIACQUISITIONQT_API SOrganSeriesListEditor() throw() ;

    /// Destructor. Do nothing.
    UIACQUISITIONQT_API virtual ~SOrganSeriesListEditor() throw() ;

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

} // uiAcquisition

#endif /*__UIACQUISITIONQT_SORGANSERIESLISTEDITOR_HPP__*/


