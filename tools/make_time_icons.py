"""강화 화면 시간대 아이콘 (낮·노을·밤): 원형 메달 18x18 도트. 테두리는 같고 안쪽 장면만 다르다.
수동 실행: python -B tools/make_time_icons.py  → assets/graphics/forge/time_icon_day·sunset·night.png
(강화 화면 배경 그림에 구워 넣는다: tools/make_forge_bg_time.py --write)
"""
import os
from PIL import Image, ImageDraw
import math
S=18; C=(S-1)/2
OUT=(36,20,14); RIM_D=(112,70,34); RIM_M=(176,122,58); RIM_L=(236,196,112)
def dist(x,y,cx=C,cy=C): return math.hypot(x-cx,y-cy)
def lerp(a,b,t): return tuple(round(a[i]+(b[i]-a[i])*t) for i in range(3))
def grad(stops,t):
    for (t0,c0),(t1,c1) in zip(stops,stops[1:]):
        if t0<=t<=t1: return lerp(c0,c1,(t-t0)/max(1e-9,t1-t0))
    return stops[-1][1]
def bands(c,n=5):  # 팔레트를 아끼려고 그라데이션을 n단계로 끊는다
    return c
def medal(sky_fn, deco_fn):
    im=Image.new('RGBA',(S,S),(0,0,0,0)); px=im.load()
    for y in range(S):
        for x in range(S):
            r=dist(x,y)
            if r>8.9: continue
            if r>8.0: px[x,y]=OUT+(255,)
            elif r>6.6:
                # 테두리: 왼쪽 위가 밝고 오른쪽 아래가 어둡다
                lit=((x-C)*-0.7+(y-C)*-0.7)/8.0
                px[x,y]=(RIM_L if lit>0.45 else RIM_M if lit>-0.25 else RIM_D)+(255,)
            elif r>6.0: px[x,y]=OUT+(255,)
            else: px[x,y]=sky_fn(x,y)+(255,)
    deco_fn(px)
    return im
def put(px,x,y,c):
    if 0<=x<S and 0<=y<S and dist(x,y)<=6.0: px[x,y]=c+(255,)
def disc(px,cx,cy,r,cols):
    # cols: (밝음, 보통, 그림자) 3단 음영
    for y in range(S):
        for x in range(S):
            d=dist(x,y,cx,cy)
            if d<=r:
                t=((x-cx)*0.6+(y-cy)*0.8)/max(r,1)
                c=cols[0] if t<-0.35 else cols[1] if t<0.45 else cols[2]
                put(px,x,y,c)
# ── 낮 ──
def sky_day(x,y): return grad([(0,(70,140,230)),(1,(150,205,250))],(y-3)/12)
def deco_day(px):
    for ang in range(0,360,45):
        for k in (4.6,5.4):
            put(px,round(C+math.cos(math.radians(ang))*k),round(C-0.5+math.sin(math.radians(ang))*k),(255,226,110))
    disc(px,C,C-0.5,3.2,((255,250,190),(255,214,70),(240,150,30)))
    for (x,y) in [(3,12),(4,12),(5,12),(4,13),(13,12),(14,12)]: put(px,x,y,(240,248,255))
# ── 노을 ──
def sky_sun(x,y):
    t=(y-3)/9
    return grad([(0,(86,50,122)),(0.45,(222,92,96)),(0.8,(255,170,80)),(1,(255,214,120))],t)
def deco_sun(px):
    disc_half=lambda: None
    for y in range(S):
        for x in range(S):
            d=dist(x,y,C,12.6)
            if d<=5.0 and y<=13:
                t=(y-12.6)/5.0
                put(px,x,y,(255,236,150) if t<-0.5 else (255,176,66) if t<0.2 else (240,112,44))
    # 언덕 실루엣
    for y in range(S):
        for x in range(S):
            hill=13.4+1.0*math.sin(x*0.7)+ (0.7 if x<6 else 0)
            if y>=hill and dist(x,y)<=6.0: put(px,x,y,(62,30,70) if y<hill+2 else (44,22,54))
    for (x,y) in [(4,6),(5,6),(12,5),(13,5),(12,7),(13,7)]: put(px,x,y,(255,150,140))
# ── 밤 ──
def sky_night(x,y): return grad([(0,(14,18,56)),(1,(44,56,128))],(y-3)/12)
def deco_night(px):
    # 초승달: 큰 원에서 안쪽 원을 뺀다
    for y in range(S):
        for x in range(S):
            if dist(x,y,8.2,8.6)<=4.2 and dist(x,y,10.4,7.2)>3.5:
                t=((x-8.2)*-0.7+(y-8.6)*-0.7)/4.2
                put(px,x,y,(250,250,226) if t>0.15 else (196,208,244) if t>-0.5 else (140,156,214))
    for (x,y) in [(12,5),(11,5),(13,5),(12,4),(12,6)]: put(px,x,y,(255,246,196))
    for (x,y) in [(5,4),(13,10),(4,9)]: put(px,x,y,(220,230,255))
    for y in range(S):
        for x in range(S):
            hill=13.0+1.2*math.sin(x*0.6+1)
            if y>=hill and dist(x,y)<=6.0: put(px,x,y,(10,12,36))


if __name__ == '__main__':
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    out = os.path.join(root, 'assets', 'graphics', 'forge')
    for name, icon in (('day', medal(sky_day, deco_day)), ('sunset', medal(sky_sun, deco_sun)), ('night', medal(sky_night, deco_night))):
        icon.save(os.path.join(out, 'time_icon_%s.png' % name))
    print('ok', out)
