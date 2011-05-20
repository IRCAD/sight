#ifndef _GUIQT_EDITOR_CODE_HPP_
#define _GUIQT_EDITOR_CODE_HPP_

#include <QPointer>
#include <QObject>
#include <QTextEdit>
#include <QString>
#include <QSyntaxHighlighter>

#include <fwTools/Failed.hpp>
#include <gui/editor/IEditor.hpp>


#include "guiQt/config.hpp"

namespace guiQt
{

namespace editor
{

/**
 * @class   Code
 * @brief   Code Editor working on a ::fwData::String
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 */
class GUIQT_CLASS_API Code : public QObject,public ::gui::editor::IEditor
{
    Q_OBJECT
public :

    fwCoreServiceClassDefinitionsMacro ( (Code)(::gui::editor::IEditor) ) ;

    /// Constructor. Do nothing.
    GUIQT_API Code() throw() ;

    /// Destructor. Do nothing.
    GUIQT_API virtual ~Code() throw() ;

protected:


    /// Install the layout.
    virtual void starting() throw(::fwTools::Failed);

    /// Destroy the layout
    virtual void stopping() throw(::fwTools::Failed);

    /// Management of observations ( overrides ). Update the value from the String object.
    virtual void updating( ::boost::shared_ptr< const fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed);

    /// Update the value from the String object.
    virtual void updating() throw(::fwTools::Failed);

    /// Update the value from the String object.
    virtual void swapping() throw(::fwTools::Failed);

    /// Do nothing.
    virtual void configuring() throw(fwTools::Failed);

    /// Overrides
    virtual void info( std::ostream &_sstream ) ;

protected:
    static const std::string PYTHON;
    static const std::string CPP;

protected slots:
    /**
     * @brief This method is called when the value change.
     *
     * Update the String object value and notify the modification.
     */
    void onModifyValue();

private:
    std::string                    m_language;
    QPointer< QTextEdit >          m_valueCtrl;
    QPointer< QSyntaxHighlighter > m_highlighter;
};

} // namespace editor

} // namespace guiQt

#endif /*_GUIQT_EDITOR_CODE_HPP_*/


