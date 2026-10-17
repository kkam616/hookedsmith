#ifndef HS_TIME_BAND_H
#define HS_TIME_BAND_H

// 카트리지 RTC 시각으로 하루를 낮 / 노을 / 밤으로 나눈다. 타이틀 그림과 강화 확률 보정이 같이 쓴다.
// 시계가 없거나 읽지 못하면 낮.
namespace hs
{

enum class time_band
{
    DAY,        // 06~16시
    SUNSET,     // 16~21시
    NIGHT,      // 21~06시
};

// 시(0~23)가 속한 시간대. 시계와 상관없는 순수 함수
constexpr time_band band_for_hour(int hour)
{
    if(hour >= 6 && hour < 16)
    {
        return time_band::DAY;
    }

    if(hour >= 16 && hour < 21)
    {
        return time_band::SUNSET;
    }

    return time_band::NIGHT;
}

// 강화 성공률에서 빼는 %p (낮 0 / 노을 2 / 밤 4)
constexpr int time_penalty_pct(time_band band)
{
    return band == time_band::SUNSET ? 2 : band == time_band::NIGHT ? 4 : 0;
}

[[nodiscard]] time_band current_time_band();

#ifdef HS_DEBUG
// 디버그 빌드: 강화 화면 SELECT 도움말에서 ↑ 로 시간대를 강제로 돌린다 (낮 → 노을 → 밤 → 실제 시계). 이름은 화면에 보여 주려고 돌려준다.
const char* debug_cycle_time_band();
#endif

}

#endif
