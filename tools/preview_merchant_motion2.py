"""수상한 상인 이벤트 모션 시안 2 (build/merchant_mock2/): 사진(assets/graphics/events/merchant_photo_source.webp)에 맞춘 상인 도트 +
은밀한 분위기의 연출 두 가지. 말투는 B(음침한 밀거래꾼). 확정 대사: "쉿... 비밀이오." / "이건 덤이오..." → "수상한 상인이 대금을" / "두 배로 쳐 주었다!"
  motion_sneak.gif   슬쩍 등장: 그림자(실루엣)로 살금 걸어 들어와 색이 돌아오고, 좌우를 살핀 뒤 사진·보상
  motion_whisper.gif 불쑥 + 속삭임: 오른쪽에서 재빨리 나타나 멈추고, 글자가 한 글자씩 속삭이듯 나타난 뒤 보상
사용: python -B tools/preview_merchant_motion2.py
"""
import os
import random
import sys

from PIL import Image, ImageDraw, ImageEnhance

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import preview_merchant_motion as base
from make_recipe_paper import GameFont

ROOT = base.ROOT
OUT = os.path.join(ROOT, 'build', 'merchant_mock2')
WIN = base.WIN
SCALE = 3
WHITE, BLACK, GOLD = base.WHITE, base.BLACK, base.GOLD

# 사진에서 뽑은 색
HAT = (44, 30, 78)
HAT_LIGHT = (66, 46, 108)
BAND = (222, 170, 44)
SKIN = (220, 150, 108)
SKIN_DARK = (176, 112, 80)
MUSTACHE = (150, 98, 50)
COAT = (36, 34, 54)
COAT_LIGHT = (56, 52, 78)
GLOVE = (124, 78, 44)
GLOVE_DARK = (88, 54, 30)
STRAP = (158, 104, 52)
BUCKLE = (196, 198, 210)
PACK = (156, 136, 92)
PACK_DARK = (110, 92, 58)
BEDROLL = (190, 170, 128)
POUCH_C = (136, 124, 92)
COIN = (250, 204, 52)
TROUSER = (40, 34, 44)
BOOT = (96, 60, 34)
TEETH = (250, 250, 240)


def merchant(pose='walk', step=0, flip=False, crouch=0):
    """32x32 투명 도트. pose: walk / look(좌우 살핌) / whisper(손으로 입을 가림) / give(주머니 내밀기)"""
    im = Image.new('RGBA', (32, 32), (0, 0, 0, 0))
    d = ImageDraw.Draw(im)
    bob = 1 if (pose == 'walk' and step % 2 == 1) else 0
    y0 = crouch + bob            # 웅크리면 윗몸이 내려간다
    # 배낭 + 말아 올린 이불
    d.rectangle((2, 9 + y0, 11, 24 + y0), fill=PACK_DARK)
    d.rectangle((3, 10 + y0, 10, 23 + y0), fill=PACK)
    d.rectangle((1, 5 + y0, 11, 9 + y0), fill=BEDROLL)
    d.rectangle((1, 7 + y0, 11, 7 + y0), fill=PACK_DARK)
    d.line((6, 10 + y0, 6, 23 + y0), fill=PACK_DARK)
    # 다리
    off = {0: (0, 0), 1: (-1, 1), 2: (0, 0), 3: (1, -1)}[step % 4] if pose == 'walk' else (0, 0)
    d.rectangle((13 + off[0], 25 + crouch, 16 + off[0], 27 + crouch), fill=TROUSER)
    d.rectangle((17 + off[1], 25 + crouch, 20 + off[1], 27 + crouch), fill=TROUSER)
    d.rectangle((12 + off[0], 28, 16 + off[0], 31), fill=BOOT)
    d.rectangle((17 + off[1], 28, 21 + off[1], 31), fill=BOOT)
    # 어두운 외투 + 가죽 멜빵과 은 버클
    d.rectangle((11, 13 + y0, 21, 26 + crouch), fill=COAT)
    d.rectangle((19, 13 + y0, 21, 26 + crouch), fill=COAT_LIGHT)
    d.line((12, 13 + y0, 19, 22 + y0), fill=STRAP)
    d.point((15, 17 + y0), fill=BUCKLE)
    d.point((16, 17 + y0), fill=BUCKLE)
    d.line((12, 22 + y0, 21, 22 + y0), fill=STRAP)
    # 얼굴: 수염과 능글맞은 입
    d.rectangle((13, 6 + y0, 20, 12 + y0), fill=SKIN)
    d.rectangle((13, 6 + y0, 13, 12 + y0), fill=SKIN_DARK)
    d.rectangle((14, 9 + y0, 21, 10 + y0), fill=MUSTACHE)
    d.line((15, 11 + y0, 19, 11 + y0), fill=TEETH)
    d.point((18, 7 + y0), fill=BLACK)
    d.line((17, 6 + y0, 20, 5 + y0), fill=MUSTACHE)                       # 치켜올린 눈썹
    # 모자: 넓은 챙과 금띠
    d.rectangle((9, 4 + y0, 24, 6 + y0), fill=HAT)
    d.rectangle((10, 4 + y0, 22, 4 + y0), fill=HAT_LIGHT)
    d.rectangle((13, 0 + y0, 20, 4 + y0), fill=HAT)
    d.line((13, 3 + y0, 20, 3 + y0), fill=BAND)
    # 팔과 가죽 장갑
    if pose == 'whisper':
        d.rectangle((19, 14 + y0, 22, 18 + y0), fill=COAT)
        d.rectangle((20, 9 + y0, 23, 13 + y0), fill=GLOVE)                  # 입 앞의 장갑
        d.point((21, 11 + y0), fill=GLOVE_DARK)
    elif pose == 'give':
        d.rectangle((20, 15 + y0, 26, 17 + y0), fill=COAT)
        d.rectangle((26, 15 + y0, 28, 18 + y0), fill=GLOVE)
        d.ellipse((27, 14 + y0, 31, 21 + y0), fill=POUCH_C)                # 금화가 보이는 주머니
        d.point((29, 14 + y0), fill=COIN)
        d.point((28, 15 + y0), fill=COIN)
        d.point((30, 15 + y0), fill=COIN)
    else:
        d.rectangle((19, 14 + y0, 22, 19 + y0), fill=COAT)
        d.rectangle((20, 19 + y0, 22, 21 + y0), fill=GLOVE)
    return im.transpose(Image.FLIP_LEFT_RIGHT) if flip else im


def silhouette(im):
    """색을 지운 검은 실루엣 (살금 등장용)"""
    r, g, b, a = im.split()
    flat = Image.new('RGBA', im.size, (14, 10, 22, 255))
    flat.putalpha(a)
    return flat


def real_photo():
    w, h = WIN[2] - WIN[0], WIN[3] - WIN[1]
    ph = Image.open(os.path.join(ROOT, 'assets', 'graphics', 'events', 'merchant_photo_source.webp')).convert('RGB')
    pw, ph_h = ph.size
    box = (918 - 70, 718 - 50, 918 + 70, 718 + 50)                         # 오른쪽 아래 생성기 마크를 왼쪽 그림으로 덮는다
    patch = ph.crop((box[0] - 160, box[1], box[2] - 160, box[3]))
    ph.paste(patch, (box[0], box[1]))
    return ph.resize((w, h), Image.LANCZOS)


def clip_sprite(img, frame_img, x, y, alpha=1.0):
    layer = Image.new('RGBA', img.size, (0, 0, 0, 0))
    layer.paste(frame_img, (int(x), int(y)), frame_img)
    if alpha < 1.0:
        layer.putalpha(layer.getchannel('A').point(lambda v: int(v * alpha)))
    mask = Image.new('L', img.size, 0)
    ImageDraw.Draw(mask).rectangle((WIN[0], WIN[1], WIN[2] - 1, WIN[3] - 1), fill=255)
    clipped = Image.new('RGBA', img.size, (0, 0, 0, 0))
    clipped.paste(layer, (0, 0), mask)
    b = img.convert('RGBA')
    b.alpha_composite(clipped)
    img.paste(b.convert('RGB'))


def build(variant):
    random.seed(9)
    font = GameFont('hs_font.bmp', 'hs_font.h', False)
    forge = base.forge_bg()
    room, room_coins, photo = base.room_image(), base.room_image(True), real_photo()
    frames = []
    stop_x = WIN[0] + 40

    def text(img, s, y, color=WHITE):
        font.draw(img, s, 120 - font.width(s) // 2, y, color, BLACK)

    def snap(img):
        frames.append(img.copy())

    # 1) 알림 번쩍 + 창이 열림
    for f in range(26):
        img = forge.copy()
        if f > 6:
            base.draw_window(img, room, scale=min(1.0, (f - 6) / 12))
        snap(base.flash(img, max(0, 0.8 - f * 0.08)))

    if variant == 'sneak':
        # 2) 실루엣으로 웅크려 살금살금 → 가운데쯤에서 색이 돌아온다 (56프레임)
        start_x = WIN[0] - 4
        for u in range(56):
            img = forge.copy()
            base.draw_window(img, room)
            x = start_x + (stop_x - start_x) * u / 56
            fr = merchant('walk', u // 8, crouch=2)
            if u < 28:
                fr = silhouette(fr)
            clip_sprite(img, fr, x, WIN[1] + 22, min(1.0, u / 8))
            snap(img)
        # 3) 좌우를 살핀다 (뒤돌아봄) + ▼ (A 대기)
        for f in range(84):
            img = forge.copy()
            base.draw_window(img, room)
            look = (f // 18) % 2 == 1
            clip_sprite(img, merchant('look', flip=look), stop_x, WIN[1] + 22)
            if (f // 10) % 2 == 0:
                text(img, '▼', 148, GOLD)
            snap(img)
    else:
        # 2) 오른쪽에서 재빨리 불쑥 (14프레임), 멈춰 서서 손으로 입을 가리고 속삭일 준비
        start_x = WIN[2] + 4
        for u in range(14):
            img = forge.copy()
            base.draw_window(img, room)
            x = start_x + (stop_x - start_x) * (u / 14) ** 0.6
            clip_sprite(img, merchant('walk', u // 2, flip=True), x, WIN[1] + 22)
            snap(img)
        for f in range(50):
            img = forge.copy()
            base.draw_window(img, room)
            clip_sprite(img, merchant('whisper', flip=(f < 12)), stop_x, WIN[1] + 22)
            if f > 24 and (f // 10) % 2 == 0:
                text(img, '▼', 148, GOLD)
            snap(img)

    # 4) A → 사진 번쩍 + 말 (B 말투). whisper는 한 글자씩 나타난다
    line1, line2 = '쉿... 비밀이오.', '이건 덤이오...'
    for f in range(70):
        img = forge.copy()
        base.draw_window(img, photo)
        if variant == 'sneak':
            if f > 10:
                text(img, line1, 124)
            if f > 24:
                text(img, line2, 138)
        else:
            n1 = max(0, min(len(line1), (f - 8) // 2))
            n2 = max(0, min(len(line2), (f - 8 - len(line1) * 2 - 6) // 2))
            if n1:
                text(img, line1[:n1], 124)
            if n2:
                text(img, line2[:n2], 138)
        if f > 56 and (f // 10) % 2 == 0:
            text(img, '▼', 150, GOLD)
        snap(base.flash(img, max(0, 0.9 - f * 0.12)))

    # 5) A → 금화 더미 방, 주머니를 내밀고 보상 메시지
    for f in range(72):
        img = forge.copy()
        base.draw_window(img, room_coins)
        clip_sprite(img, merchant('give'), stop_x, WIN[1] + 22)
        if f > 6:
            text(img, '수상한 상인이 대금을', 118)
            text(img, '두 배로 쳐 주었다!', 132)
            text(img, '+ $1,250', 146, GOLD)
        snap(img)

    # 6) 강화 화면으로 복귀: 금화가 튀며 소지금 상승
    coins = [(random.randint(90, 150), -random.randint(0, 40), random.uniform(-1, 1), random.uniform(0.5, 2.5)) for _ in range(18)]
    for f in range(56):
        img = base.forge_bg()
        d = ImageDraw.Draw(img)
        for (x, y0, vx, vy) in coins:
            t = f - random.Random(int(x)).randint(0, 10)
            if t < 0:
                continue
            y = y0 + vy * t * 1.6 + 0.32 * t * t
            if y < 130:
                d.ellipse((x + vx * t, y, x + vx * t + 4, y + 3), fill=(250, 210, 70), outline=(180, 120, 20))
        money = 5000 + int(1250 * min(1, f / 40))
        font.draw(img, '$%s' % format(money, ','), 236, 148, (248, 216, 48), (232, 104, 8), 'right')
        snap(img)
    return frames


def main():
    os.makedirs(OUT, exist_ok=True)
    sheets = []
    for variant in ('sneak', 'whisper'):
        frames = build(variant)
        big = [f.resize((240 * SCALE, 160 * SCALE), Image.NEAREST).quantize(colors=96, method=Image.Quantize.MEDIANCUT, dither=Image.Dither.NONE)
               for f in frames]
        big[0].save(os.path.join(OUT, 'motion_%s.gif' % variant), save_all=True, append_images=big[1:], duration=50, loop=0)
        n = len(frames)
        picks = [8, 26 + 10, 26 + 40, 26 + 70, 26 + 110, 26 + 150, n - 50]
        sheets.append([frames[min(p, n - 1)] for p in picks])
        print(variant, n)

    sheet = Image.new('RGB', (7 * 244, 2 * 164), (30, 30, 34))
    for r, row in enumerate(sheets):
        for c, f in enumerate(row):
            sheet.paste(f, (c * 244 + 2, r * 164 + 2))
    sheet.resize((sheet.width, sheet.height)).save(os.path.join(OUT, 'sheet.png'))

    names = [('walk', 0, False, 0), ('walk', 1, False, 0), ('walk', 2, False, 0), ('walk', 3, False, 0), ('walk', 0, False, 2),
             ('look', 0, True, 0), ('whisper', 0, False, 0), ('give', 0, False, 0)]
    strip = Image.new('RGBA', (32 * 8 * 6, 32 * 6), (70, 70, 80, 255))
    for i, (p, s, fl, cr) in enumerate(names):
        strip.alpha_composite(merchant(p, s, fl, cr).resize((192, 192), Image.NEAREST), (i * 192, 0))
    strip.save(os.path.join(OUT, 'merchant_frames.png'))

    photo = real_photo().resize((86 * 4, 70 * 4), Image.NEAREST)
    photo.save(os.path.join(OUT, 'photo_in_window.png'))


if __name__ == '__main__':
    main()
