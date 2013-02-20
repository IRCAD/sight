/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIMEDDATAQT_EDITOR_SSELECTOR_HPP_
#define _UIMEDDATAQT_EDITOR_SSELECTOR_HPP_

#include <QPointer>
#include <QObject>
#include <QVector>

#include <fwData/Vector.hpp>

#include <gui/editor/IEditor.hpp>

#include "uiMedDataQt/config.hpp"
#include "uiMedDataQt/widget/Selector.hpp"


namespace uiMedData
{
namespace editor
{
/**
 * @brief   This editor shows information about the medical data. It allows to manipulate
 *          (select, erase, ...) studies and series.
 * @class   SSelector.
 * @author  IRCAD (Research and Development Team).
 * @date    2013.
 */
class UIMEDDATAQT_CLASS_API SSelector : public QObject, public ::gui::editor::IEditor
{
    Q_OBJECT
public :
    fwCoreServiceClassDefinitionsMacro ( (SSelector)(::gui::editor::IEditor) ) ;

    /// Constructor
    UIMEDDATAQT_API SSelector();

    /// Destructor
    UIMEDDATAQT_API virtual ~SSelector() throw() ;

protected:

    /// Installs GUI : create container and add selector.
    virtual void starting() throw(::fwTools::Failed);

    /// Destroys GUI.
    virtual void stopping() throw(::fwTools::Failed);

    /**
     *
     * @verbatim
     <service uid="selector" impl="::uiMedData::editor::SSelector" type="::gui::editor::IEditor" autoConnect="yes">
        <selectionId>selections</selectionId>
     </service>
     @endverbatim
     * - \<selectionId>selections</\selectionId> : defines the fwId of the ::fwData::Vector where the selection
     * will be put or get.
     */
    virtual void configuring() throw (::fwTools::Failed);

    /// Fill selector with the series contained in SeriesDB.
    virtual void updating() throw (::fwTools::Failed);

    /** Manages events (::fwComEd::SeriesDBMsg::ADDED_OBJECTS, ::fwComEd::SeriesDBMsg::REMOVED_OBJECTS)
     *  This method adds/removes series in the selector widget.
     */
    virtual void receiving( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);

    virtual void info( std::ostream &_sstream );



protected Q_SLOTS:

    /**
     * @brief Manages the selection vector according to selection/deselection.
     * @param[in] selection series to add in selection vector.
     * @param[in] deselection series to remove from selection vector.
     */
    void onSelectedSeries(QVector< ::fwMedData::Series::sptr > selection,
                          QVector< ::fwMedData::Series::sptr > deselection);

    /**
     * @brief Shows a simple message dialog with the current series uid (if a series is double clicked).
     * @todo  Sends signal to activity launcher.
     */
    void onDoubleClick(const QModelIndex &index);

    /**
     * @brief Removes series from seriesDB and notify.
     * @param[in] selection series to remove from seriesDB.
     */
    void onRemoveSeries(QVector< ::fwMedData::Series::sptr > selection);

private :

    /// Returns current selection vector given by its fwID m_selectionId.
    ::fwData::Vector::sptr getSelection();

    /// fwID of the vector of selections
    std::string m_selectionId;

    /// Selector widget.
    QPointer< ::uiMedData::widget::Selector > m_selectorWidget;

};
} // namespace editor
} // namespace uiMedData

#endif // _UIMEDDATAQT_EDITOR_SSELECTOR_HPP_

