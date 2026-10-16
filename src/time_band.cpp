#include "time_band.h"

#include "bn_optional.h"
#include "bn_time.h"

namespace hs
{

time_band current_time_band()
{
    if(bn::time::active())
    {
        if(bn::optional<bn::time> now = bn::time::current())
        {
            return band_for_hour(now->hour());
        }
    }

    return time_band::DAY;
}

}
