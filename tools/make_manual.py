"""docs/GAME_MANUAL.md 안의 AUTO 표를 include/economy_data.h 에서 다시 만든다.

수동 실행: python tools/make_manual.py
매뉴얼의 <!-- AUTO:이름 --> ... <!-- /AUTO:이름 --> 사이만 바뀐다. 그 밖의 글은 손으로 쓴다.
"""
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DATA = ROOT / "include" / "economy_data.h"
MANUAL = ROOT / "docs" / "GAME_MANUAL.md"

sys.stdout.reconfigure(encoding="utf-8")
sys.stderr.reconfigure(encoding="utf-8")

src = DATA.read_text(encoding="utf-8")


def block(name):
    """`name[...] = {` 부터 `};` 까지의 본문."""
    m = re.search(name + r"[^=]*=\s*\{(.*?)\n\};", src, re.S)
    if not m:
        m = re.search(name + r"[^=]*=\s*\{(.*?)\};", src, re.S)
    return m.group(1)


def strings(text):
    return re.findall(r'"([^"]*)"', text)


def const_int(name):
    return int(re.search(name + r"\s*=\s*([\d']+)", src).group(1).replace("'", ""))


mat_ids = [m for m in re.findall(r"^\s*([A-Z_]+)(?:\s*=\s*-?\d+)?,", re.search(r"enum class material[^{]*\{(.*?)\};", src, re.S).group(1), re.M) if m != "NONE"]
mat_names = strings(block("material_names"))
mat = dict(zip(mat_ids, mat_names))
sword_names = strings(block("sword_names"))

ROW = re.compile(
    r"\{\s*(-?\d+),\s*(-?\d+),\s*(-?\d+),\s*(-?\d+),\s*material::(\w+)"
    r"(?:,\s*material::(\w+),\s*(-?\d+),\s*(-?\d+))?\s*\}"
)


def rows(name):
    out = []
    for m in ROW.finditer(block(name)):
        cost, pct, sell, prot, drop, cmat, csword, cqty = m.groups()
        out.append(dict(cost=int(cost), pct=int(pct), sell=int(sell), prot=int(prot), drop=drop,
                        cmat=cmat or "NONE", csword=int(csword or 0), cqty=int(cqty or 0)))
    return out


easy = rows("easy_levels")
hard = rows("hard_levels")
assert len(easy) == len(hard) == len(sword_names) == const_int("max_level") + 1, "행 수가 다르다"


def money(v):
    return f"${v:,}"


def cost_text(r):
    if r["csword"] > 0:
        return f"보관 +{r['csword']}검 ×{r['cqty']}"
    if r["cmat"] != "NONE":
        return f"{mat[r['cmat']]} ×{r['cqty']}"
    return money(r["cost"]) if r["cost"] > 0 else "무료"


def sell_text(r):
    return {0: "없음", -1: "불가"}.get(r["sell"], money(r["sell"]))


def prot_text(r):
    return "-" if r["prot"] == 0 else ("불가" if r["prot"] < 0 else f"{r['prot']}장")


def t_levels():
    out = ["| 단계 | 검 이름 | 모드 | 성공률 | 강화 비용 | 판매가 | 방지권 | 파괴 시 잡템 |",
           "|---|---|---|---|---|---|---|---|"]
    for i, name in enumerate(sword_names[:-1]):
        for label, t in (("STORY", easy), ("HARD", hard)):
            r = t[i]
            first = label == "STORY"
            out.append("| {} | {} | {} | {}% | {} | {} | {} | {} |".format(
                f"+{i}→{i + 1}" if first else "", name if first else "", label, r["pct"],
                cost_text(r), sell_text(r), prot_text(r),
                mat[r["drop"]] if r["drop"] != "NONE" else "-"))
    last = len(sword_names) - 1
    out.append(f"| +{last} | {sword_names[last]} | | 최종 검 (엔딩) | | | | |")
    return "\n".join(out)


def cumulative(t, upto):
    p = 1.0
    for r in t[:upto]:
        p *= r["pct"] / 100
    return p * 100


def fmt_pct(p):
    if p >= 1:
        return f"{p:.1f}%"
    if p >= 0.0001:
        return f"{p:.4f}%"
    return "0.0001% 미만"


def t_cumulative():
    out = ["| 목표 | STORY | HARD |", "|---|---|---|"]
    for goal in (6, 10, 13, 15, 20, 25, 29):
        out.append(f"| +{goal} | {fmt_pct(cumulative(easy, goal))} | {fmt_pct(cumulative(hard, goal))} |")
    return "\n".join(out)


def t_drop():
    pcts = [int(x) for x in re.search(r"drop_count_pct\[\]\s*=\s*\{([^}]*)\}", src).group(1).split(",")]
    avg = sum(i * p for i, p in enumerate(pcts)) / sum(pcts)
    out = ["| 잡템 개수 | " + " | ".join(f"{i}개" for i in range(len(pcts))) + " |",
           "|---|" + "---|" * len(pcts),
           "| 확률 | " + " | ".join(f"{p}%" for p in pcts) + f" |\n\n평균 {avg:.2f}개 (임시값)"]
    return "\n".join(out)


def t_shop():
    out = ["| 상품 | STORY | HARD | 효과 |", "|---|---|---|---|"]
    for m in re.finditer(r'\{\s*"([^"]*)",\s*(\d+),\s*(\d+),\s*shop_effect::(\w+),\s*(\d+)\s*\}', block("shop_items")):
        name, hard_usd, easy_usd, eff, amt = m.groups()
        effect = f"방지권 +{amt}장" if eff == "PROTECT" else f"+0 검일 때 {warp_pct}% 확률로 +{amt} 검"
        out.append(f"| {name} | {money(int(easy_usd))} | {money(int(hard_usd))} | {effect} |")
    return "\n".join(out)


def t_recipes():
    out = ["| 재료 | STORY 필요 | HARD 필요 | 결과 (STORY) | 결과 (HARD) |", "|---|---|---|---|---|"]
    pat = r"\{\s*material::(\w+),\s*(\d+),\s*(\d+),\s*craft_output::(\w+),\s*(\d+),\s*(\d+)\s*\}"
    for m in re.finditer(pat, block("recipes")):
        inp, hq, eq, outk, ha, ea = m.groups()

        def res(a):
            return f"방지권 {a}장" if outk == "PROTECT" else f"+{a} 검 (창고로)"
        out.append(f"| {mat[inp]} | {eq} | {hq} | {res(ea)} | {res(ha)} |")
    return "\n".join(out)


def t_materials():
    drops = {}
    for i, r in enumerate(easy):
        if r["drop"] != "NONE":
            drops.setdefault(r["drop"], []).append(i)
    out = ["| 잡템 | 떨어지는 단계 | 쓰이는 곳 |", "|---|---|---|"]
    for mid in mat_ids:
        lv = drops.get(mid, [])
        where = f"+{lv[0]}" + (f"~+{lv[-1]}" if len(lv) > 1 else "") if lv else "-"
        uses = []
        if re.search(rf"\{{\s*material::{mid},\s*\d+,\s*\d+,\s*craft_output", block("recipes")):
            uses.append("화구")
        for i, r in enumerate(easy):
            if r["cmat"] == mid:
                uses.append(f"+{i}→{i + 1} 강화 재료")
        out.append(f"| {mat[mid]} | {where} | {', '.join(uses) or '-'} |")
    return "\n".join(out)


def t_constants():
    return "\n".join([
        "| 항목 | 값 | 비고 |", "|---|---|---|",
        f"| 시작 소지금 (STORY) | {money(const_int('start_money_easy'))} | 임시 (원작 100만원 ÷ 100) |",
        f"| 시작 소지금 (HARD) | {money(const_int('start_money_hard'))} | 임시 (원작 200만원 ÷ 100) |",
        f"| 소지금 상한 | {money(const_int('money_cap'))} | 넘치면 클램프 |",
        f"| 창고 칸 수 | {const_int('max_stored_swords')}칸 | 임시 |",
        f"| 워프 성공 확률 | {warp_pct}% | 임시 (원작에 명시 없음) |",
        "| 잡템 보유 상한 | 종류당 999개 | `economy.cpp` |",
        f"| 최종 단계 | +{const_int('max_level')} | 엔딩 |",
    ])


warp_pct = const_int("warp_success_pct")

TABLES = dict(levels=t_levels, cumulative=t_cumulative, drop=t_drop, shop=t_shop,
              recipes=t_recipes, materials=t_materials, constants=t_constants)

text = MANUAL.read_text(encoding="utf-8")
for key, fn in TABLES.items():
    pat = re.compile(rf"(<!-- AUTO:{key} -->\n).*?(<!-- /AUTO:{key} -->)", re.S)
    if not pat.search(text):
        print(f"경고: 매뉴얼에 AUTO:{key} 자리가 없다", file=sys.stderr)
        continue
    text = pat.sub(lambda m: m.group(1) + fn() + "\n" + m.group(2), text)

MANUAL.write_text(text, encoding="utf-8", newline="\n")
print("GAME_MANUAL.md 갱신")
