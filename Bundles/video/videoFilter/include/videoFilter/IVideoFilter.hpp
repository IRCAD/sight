#ifndef __VIDEOFILTER_IVIDEOFILTER_HPP__
#define __VIDEOFILTER_IVIDEOFILTER_HPP__

#include "videoFilter/config.hpp"

#include <fwServices/IService.hpp>

namespace videoFilter
{
/**
 * @brief   This interface defines video filters.
 *          Must be implemented for services that filter videos.
 *
 */
class VIDEOFILTER_CLASS_API IVideoFilter : public ::fwServices::IService
{

public:

    fwCoreServiceClassDefinitionsMacro((IVideoFilter)(::fwServices::IService));

    /**
     * @brief Constructor.
     */
    IVideoFilter() noexcept
    {
    }

    /**
     * @brief Destructor.
     */
    virtual ~IVideoFilter() noexcept
    {
    }

};

} //namespace videoFilter
#endif  // __VIDEOFILTER_IVIDEOFILTER_HPP__
