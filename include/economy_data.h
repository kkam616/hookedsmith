// 경제 밸런스 데이터. 출처: docs/ECONOMY_SPEC.md (원작 원화 ÷100 = 달러, 임시 밸런스)
// 수치 조정은 이 파일에서만 한다. 로직은 economy.h / economy.cpp.

#ifndef HS_ECONOMY_DATA_H
#define HS_ECONOMY_DATA_H

#include <cstdint>

namespace hs
{

enum class difficulty : uint8_t
{
    EASY,   // STORY
    HARD,   // INFINITE
};

enum class material : int8_t
{
    NONE = -1,
    SCRAP_UNKNOWN,
    TAU_BONE,
    FADED_GLOW,
    SCRAP_SWISS,
    FLAME_HILT,
    EVIL_SOUL,
    AXE_DUST,
    CLEAR_MATTER,
};

constexpr int material_count = 8;

// 자리표시 이름. 메시지 줄 폭 때문에 4글자(공백 포함 5칸) 이하로 둔다.
constexpr const char* material_names[material_count] = {
    "낡은 고철",
    "타우 뼈",
    "바랜 빛",
    "스위스 철",
    "불꽃 자루",
    "악령 혼",
    "도끼 가루",
    "투명 물질",
};

struct level_row
{
    int32_t cost_usd;       // 강화 비용
    int8_t success_pct;     // 성공 확률
    int32_t sell_usd;       // 판매가 (0 = 판매 불가)
    int8_t protect_cost;    // 실패 시 방지권 장수 (0 이하 = 사용 불가)
    material fail_drop;     // 파괴 시 드랍 잡템
};

// 우리 게임은 +20 달성이 엔딩. +21~+28(재료 강화) 행은 스펙에만 두고 아직 넣지 않는다.
constexpr int max_level = 20;
constexpr int level_count = max_level + 1;

constexpr level_row hard_levels[level_count] = {
    {    5, 100,       0,  0, material::NONE },          // +0
    {    5,  98,       2,  0, material::NONE },
    {   10,  95,       5,  0, material::NONE },
    {   20,  93,      10,  0, material::NONE },
    {   40,  90,      20,  1, material::NONE },
    {   70,  86,      60,  1, material::NONE },          // +5
    {  100,  81,     150,  1, material::SCRAP_UNKNOWN },
    {  150,  75,     250,  1, material::SCRAP_UNKNOWN },
    {  220,  70,     500,  1, material::SCRAP_UNKNOWN },
    {  300,  66,     900,  1, material::TAU_BONE },
    {  300,  62,    1800,  1, material::FADED_GLOW },    // +10
    {  510,  61,    5000,  1, material::SCRAP_SWISS },
    {  700,  54,   10000,  1, material::SCRAP_SWISS },
    {  800,  50,   20000,  2, material::FLAME_HILT },
    { 1000,  49,   50000,  3, material::EVIL_SOUL },
    { 1300,  46,  100000,  4, material::AXE_DUST },      // +15
    { 1700,  44,  200000,  7, material::CLEAR_MATTER },
    { 2200,  40,  445000,  9, material::NONE },
    { 3000,  38,  720000, 10, material::NONE },
    { 4000,  35, 1200000, 12, material::NONE },
    { 6500,  33, 2400000, 15, material::NONE },          // +20
};

constexpr level_row easy_levels[level_count] = {
    {     3, 100,      0,  0, material::NONE },          // +0
    {     3, 100,      2,  0, material::NONE },
    {     5, 100,      4,  0, material::NONE },
    {     5,  95,      6,  0, material::NONE },
    {    10,  95,      8,  0, material::NONE },
    {    15,  90,     16,  0, material::NONE },          // +5
    {    20,  90,     35,  1, material::SCRAP_UNKNOWN },
    {    20,  90,     61,  1, material::SCRAP_UNKNOWN },
    {    30,  85,    100,  1, material::SCRAP_UNKNOWN },
    {    50,  80,    200,  1, material::TAU_BONE },
    {   109,  80,    351,  1, material::FADED_GLOW },    // +10
    {   200,  75,   1600,  1, material::SCRAP_SWISS },
    {   350,  70,   3500,  1, material::SCRAP_SWISS },
    {   550,  70,  10000,  2, material::FLAME_HILT },
    {  1000,  65,  30000,  3, material::EVIL_SOUL },
    {  1800,  60,  75000,  4, material::AXE_DUST },      // +15
    {  3000,  60, 142000,  7, material::CLEAR_MATTER },
    {  3000,  55, 200000,  9, material::NONE },
    {  5000,  50, 300000, 10, material::NONE },
    {  8000,  50, 475000, 12, material::NONE },
    { 15000,  45, 683000, 15, material::NONE },          // +20
};

constexpr const level_row* level_table(difficulty diff)
{
    return diff == difficulty::HARD ? hard_levels : easy_levels;
}

// 파괴 시 잡템 드랍 수량 [min, max]
constexpr int drop_min = 1;
constexpr int drop_max = 3;

// 스펙에 없는 임시값
constexpr int32_t start_money = 100;            // 시작 소지금
constexpr int32_t loan_money = 50;              // +0인데 강화비가 없을 때 이웃이 주는 돈
constexpr int32_t money_cap = 999'999'999;      // 소지금 상한
constexpr int max_stored_swords = 8;            // 보관함 칸 수

// 상점
enum class shop_effect : uint8_t
{
    PROTECT,    // amount = 방지권 장수
    WARP,       // amount = 목표 단계. +0 검일 때만, warp_success_pct 확률로 그 단계가 된다
};

struct shop_item
{
    const char* name;
    int32_t hard_usd;
    int32_t easy_usd;
    shop_effect effect;
    int8_t amount;
};

constexpr shop_item shop_items[] = {
    { "방지권 1장", 21000, 10000, shop_effect::PROTECT, 1 },
    { "방지권 3장", 60000, 25000, shop_effect::PROTECT, 3 },
    { "워프 +9",     8000, 10000, shop_effect::WARP, 9 },
    { "워프 +13",   50000, 70000, shop_effect::WARP, 13 },
    { "워프 +14",   75000, 100000, shop_effect::WARP, 14 },
    { "워프 +15",  100000, 150000, shop_effect::WARP, 15 },
};

constexpr int shop_item_count = sizeof(shop_items) / sizeof(shop_items[0]);

// 원작에 명시가 없어 임시
constexpr int warp_success_pct = 50;

// 조합소 (잡템 → 방지권 또는 보관 검)
enum class craft_output : uint8_t
{
    PROTECT,    // amount = 방지권 장수
    SWORD,      // amount = 검 단계 (보관함으로)
};

struct recipe
{
    material input;
    int8_t hard_qty;
    int8_t easy_qty;
    craft_output output;
    int8_t hard_amount;
    int8_t easy_amount;
};

constexpr recipe recipes[] = {
    { material::SCRAP_UNKNOWN, 8, 5, craft_output::PROTECT, 1, 1 },
    { material::TAU_BONE,      5, 3, craft_output::PROTECT, 1, 1 },
    { material::FADED_GLOW,    3, 2, craft_output::SWORD,  13, 13 },
    { material::SCRAP_SWISS,   5, 3, craft_output::PROTECT, 2, 2 },
    { material::FLAME_HILT,    2, 2, craft_output::SWORD,  16, 16 },
    { material::EVIL_SOUL,     4, 6, craft_output::SWORD,  19, 19 },
    { material::AXE_DUST,      6, 6, craft_output::PROTECT, 10, 10 },
    { material::CLEAR_MATTER,  4, 3, craft_output::PROTECT, 11, 9 },
};

constexpr int recipe_count = sizeof(recipes) / sizeof(recipes[0]);

}

#endif
