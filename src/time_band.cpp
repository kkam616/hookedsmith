#include "time_band.h"

#include "bn_optional.h"
#include "bn_time.h"

namespace hs
{

#ifdef HS_DEBUG
namespace
{
    int debug_band = -1;        // -1 = 실제 시계
}

const char* debug_cycle_time_band()
{
    debug_band = debug_band >= 2 ? -1 : debug_band + 1;
    return debug_band < 0 ? "실제 시계" : debug_band == 0 ? "낮" : debug_band == 1 ? "노을" : "밤";
}
#endif

time_band current_time_band()
{
#ifdef HS_DEBUG
    if(debug_band >= 0)
    {
        return time_band(debug_band);
    }
#endif
#ifdef HS_FORCE_BAND
    return time_band(HS_FORCE_BAND);       // 화면 확인용 빌드: 시계와 상관없이 0 낮 / 1 노을 / 2 밤
#endif

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
