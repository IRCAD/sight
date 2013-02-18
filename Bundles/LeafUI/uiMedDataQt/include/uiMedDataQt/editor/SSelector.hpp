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
 * @brief   This editor shows information about the medical data. It allows to manipulate (select, erase, ...)studies and series.
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

    virtual void starting() throw(::fwTools::Failed);

    virtual void stopping() throw(::fwTools::Failed);

    /**
     *
     * @verbatim
     <service uid="selector" impl="::uiMedData::editor::SSelector" type="::gui::editor::IEditor" autoConnect="yes">
        <selectionId>selections</selectionId>
     </service>
     @endverbatim
     */
    virtual void configuring() throw (::fwTools::Failed);

    virtual void updating() throw (::fwTools::Failed);

    virtual void receiving( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);

    virtual void info( std::ostream &_sstream );



protected Q_SLOTS:
    void onSelectedSeries(QVector< ::fwMedData::Series::sptr > selection,
                          QVector< ::fwMedData::Series::sptr > deselection);
    void onDoubleClick(const QModelIndex &index);

private :

    ::fwData::Vector::sptr getSelection();

    std::string m_selectionId;

    QPointer< ::uiMedData::widget::Selector > m_selectorWidget;

};
} // namespace editor
} // namespace uiMedData

#endif // _UIMEDDATAQT_EDITOR_SSELECTOR_HPP_

