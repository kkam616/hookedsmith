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
    HARD,   // 타이틀의 HARD
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
    // 재료 강화(+21~): 돈 대신(또는 돈과 함께) 소모하는 재료. 잡템이면 cost_material, 보관한 검이면 cost_sword(단계).
    material cost_material = material::NONE;
    int8_t cost_sword = 0;
    int8_t cost_qty = 0;
};

// 원작 최종 단계 +29가 엔딩. +29는 더 올릴 수 없는 마지막 검이라 행에는 빈 값만 둔다.
constexpr int max_level = 29;
constexpr int level_count = max_level + 1;

// 검 이름 (+0~+29). 원작 이름을 한글로 쓴다.
constexpr const char* sword_names[level_count] = {
    "낡은 단검",
    "쓸만한 단검",
    "견고한 단검",
    "바이킹 소드",
    "불타는 검",
    "냉기의 소드",
    "양날 검",
    "심판자의 대검",
    "마력의 검",
    "타우 스워드",
    "형광검",
    "피묻은 검",
    "화염의 쌍검",
    "불꽃 마검",
    "마검 아포피스",
    "데몬 배틀 엑스",
    "투명 검",
    "날렵한 용검",
    "샤이니 소드",
    "왕푸야샤",
    "다색검",
    "템페스트 골드",
    "샤프 워커",
    "피에로의 쌍검",
    "도룡도",
    "안 강해보이는 검",
    "메두사",
    "오딧세이 소드",
    "모자이칼",
    "화염에 달군 검",
};

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
    {    0,  30, 3000000, 17, material::NONE, material::NONE, 19, 1 },          // +21: 보관한 +19 검 1개
    {    0,  27, 4000000, 20, material::NONE, material::NONE, 21, 2 },
    {    0,  27, 5500000, 22, material::NONE, material::EVIL_SOUL, 0, 12 },
    {    0,  25, 7500000, 23, material::NONE, material::NONE, 22, 1 },
    {    0,  35, 4000000, 23, material::NONE, material::AXE_DUST, 0, 15 },      // +25
    { 50000,  50, 18000000, -1, material::NONE },
    {    0,  40, 25000000, -1, material::NONE, material::CLEAR_MATTER, 0, 2 },
    {    0,  15,       -1, -1, material::NONE },                                // +28: 무료, 판매 불가
    {    0,   0,        0, -1, material::NONE },                                // +29: 최종 (엔딩)
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
    {     0,  40, 1010000, 17, material::NONE, material::NONE, 19, 1 },         // +21
    {     0,  40, 1600000, 20, material::NONE, material::NONE, 21, 2 },
    {     0,  40, 2300000, 22, material::NONE, material::EVIL_SOUL, 0, 8 },
    {     0,  40, 3000000, 23, material::NONE, material::NONE, 22, 1 },
    {     0,  35, 4000000, 23, material::NONE, material::AXE_DUST, 0, 15 },     // +25
    { 50000,  50, 18000000, -1, material::NONE },
    {     0,  40, 25000000, -1, material::NONE, material::CLEAR_MATTER, 0, 2 },
    {     0,  15,        -1, -1, material::NONE },                              // +28
    {     0,   0,         0, -1, material::NONE },                              // +29
};

constexpr const level_row* level_table(difficulty diff)
{
    return diff == difficulty::HARD ? hard_levels : easy_levels;
}

// 파괴 시 잡템 드랍 수량 0~5개의 확률(%). 합이 100. 평균 약 2.1개 (임시값, 조정 가능)
constexpr int drop_count_pct[] = { 10, 25, 30, 20, 10, 5 };

// 스펙에 없는 임시값
constexpr int32_t start_money_easy = 10000;     // 스토리 시작 소지금 (100만원 ÷ 100)
constexpr int32_t start_money_hard = 20000;     // 하드 시작 소지금 (200만원 ÷ 100)
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

// 고양이 이벤트: 강화 실패 횟수가 목표에 닿으면 고양이가 보은으로 검(없으면 방지권)을 물어온다.
// 목표는 시작할 때와 고양이가 다녀갈 때마다 아래 셋 중에서 무작위로 정한다. 디버그 빌드는 바로 확인하도록 2·3·4번이다.
#ifdef HS_DEBUG
constexpr int cat_targets[3] = { 2, 3, 4 };
#else
constexpr int cat_targets[3] = { 26, 30, 62 };
#endif
constexpr int cat_gift_levels[4] = { 17, 18, 20, 21 };      // 선물로 받는 검 단계 (이 중 무작위)
constexpr int cat_gift_tickets = 3;

// 도둑 이벤트 (밤에만): 밤에 강화·판매를 마칠 때마다 낮은 확률로 판정한다. 훔칠 것이 없거나 쿨다운 중이면 안 온다.
// 창고의 +17 이상 검 하나를 무작위로 훔쳐 가고, 그런 검이 없으면 소지금의 20%를 훔친다. 디버그 빌드는 바로 확인하도록 확률 100%·쿨다운 짧게.
#ifdef HS_DEBUG
constexpr int thief_chance_pct = 100;
constexpr int thief_cooldown_actions = 2;
#else
constexpr int thief_chance_pct = 4;
constexpr int thief_cooldown_actions = 20;           // 다녀간 뒤 밤에 강화·판매를 이만큼 해야 다시 올 수 있다
#endif
// 수상한 상인 이벤트 (노을에만): 노을(16~21시)에 검을 판매할 때마다 낮은 확률로 판정한다. 방금 판 금액의 100%를 더 준다 (상한 없음, STORY·HARD 공통).
// 다녀간 뒤 노을에 판매를 쿨다운만큼 해야 다시 올 수 있다 (저장 안 함). 디버그 빌드는 바로 확인하도록 확률 100%·쿨다운 짧게.
#ifdef HS_DEBUG
constexpr int merchant_chance_pct = 100;
constexpr int merchant_cooldown_sales = 2;
#else
constexpr int merchant_chance_pct = 2;
constexpr int merchant_cooldown_sales = 10;
#endif

constexpr int thief_min_sword_level = 17;
constexpr int thief_money_pct = 20;                         // 창고가 가득 찼을 때 대신 받는 방지권

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
    { material::EVIL_SOUL,     4, 4, craft_output::PROTECT, 4, 4 },
    { material::EVIL_SOUL,     4, 6, craft_output::SWORD,  19, 19 },
    { material::AXE_DUST,      6, 6, craft_output::PROTECT, 10, 10 },
    { material::CLEAR_MATTER,  4, 3, craft_output::PROTECT, 11, 9 },
};

constexpr int recipe_count = sizeof(recipes) / sizeof(recipes[0]);

}

#endif
