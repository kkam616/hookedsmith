# 디버그 ROM으로 랜덤 이벤트(thief / cat / merchant)를 녹화해 build\rec\<이름>_new 에 프레임을 저장한다.
# 사용: powershell -File tools\rec_events.ps1 thief   (프로젝트 폴더에서, mGBA 창을 가리지 말 것)
param([string]$Scenario = 'thief')
Set-Location C:\gba\hookedsmith
Get-Process mGBA -EA SilentlyContinue | Stop-Process
Start-Sleep 2
. tools\mgba_auto.ps1
. tools\record_helper.ps1
Start-Game 'hookedsmith_debug.gba' | Out-Null
Start-Sleep 6
Press $VK.START; Start-Sleep 1
Press $VK.START; Start-Sleep 1.5
Press $VK.RIGHT; Start-Sleep -Milliseconds 500      # HARD
Press $VK.START; Start-Sleep 1.5
Press $VK.A; Start-Sleep 2                           # NEW GAME

function Set-Band($ups) {
    Press $VK.SELECT; Start-Sleep 1
    foreach ($i in 1..$ups) { Press $VK.UP; Start-Sleep -Milliseconds 500 }
    Press $VK.SELECT; Start-Sleep 1.5
}

if ($Scenario -eq 'thief') {
    Set-Band 3                                       # 밤
    Press $VK.A
    $pre = @(@{ t = 100000; vk = $VK.A; hold = 100 })
    Rec-Auto $pre $null 0 0 @(3000, 5400, 7600, 9800) 1800
}
elseif ($Scenario -eq 'cat') {
    Set-Band 1                                       # 낮
    Press $VK.A; Start-Sleep 3.5                     # +1
    Press $VK.R; Start-Sleep 1                       # 무조건 실패
    $pre = @()
    $pre = @(@{ t = 100000; vk = $VK.A; hold = 100 })
    Rec-Auto $pre $VK.A 0 2700 @(3000, 5400, 7600, 9800) 1800
}
else {
    Set-Band 2                                       # 노을
    Press $VK.A; Start-Sleep 3.5                     # +1
    Press $VK.B                                      # 판매 → 상인
    $pre = @(@{ t = 100000; vk = $VK.B; hold = 100 })
    Rec-Auto $pre $null 0 0 @(4200, 6600, 8800, 11000) 1800
}
"start=$($script:recStart) frames=$($script:recFrames.Count)"
Rec-Save "build\rec\${Scenario}_new" | Out-Null
