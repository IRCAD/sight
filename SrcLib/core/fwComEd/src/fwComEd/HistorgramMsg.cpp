#include <fwTools/ClassRegistrar.hpp>

#include "fwComEd/HistogramMsg.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwComEd::HistogramMsg, ::fwComEd::HistogramMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string HistogramMsg::VALUE_IS_MODIFIED = "VALUE_IS_MODIFIED";

//-----------------------------------------------------------------------------

HistogramMsg::HistogramMsg() throw()
{}

//-----------------------------------------------------------------------------

HistogramMsg::~HistogramMsg() throw()
{}

//-----------------------------------------------------------------------------

} // namespace fwComEd

