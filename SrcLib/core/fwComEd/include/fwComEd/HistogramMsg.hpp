#ifndef _FWCOMED_FLOAT_MSG_HPP_
#define _FWCOMED_FLOAT_MSG_HPP_

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/export.hpp"


namespace fwComEd
{

/**
 * @brief   Object event message specialized for histogram field.
 * @class   HistogramMsg
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 * @see     ::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API HistogramMsg : public fwServices::ObjectMsg
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((HistogramMsg)(::fwServices::ObjectMsg), ( ( ) ), new HistogramMsg );

    /// Event identifier used to inform that the value is modified.
    FWCOMED_API static std::string VALUE_IS_MODIFIED;

    /**
     * @brief   Constructor : does nothing.
     */
    FWCOMED_API HistogramMsg() throw();

    /**
     * @brief   Destructor : does nothing.
     */
    FWCOMED_API virtual ~HistogramMsg() throw();
};


} // namespace fwComEd

#endif //_FWCOMED_HISTOGRAM_MSG_HPP_

