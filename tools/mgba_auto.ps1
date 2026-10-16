# mGBA 자동 조작 도우미: 창에 키 메시지를 직접 보내고(PostMessage) F12로 스크린샷을 찍는다.
Add-Type -Namespace MG -Name Win -MemberDefinition @'
[DllImport("user32.dll")] public static extern bool PostMessage(System.IntPtr h, uint m, System.IntPtr w, System.IntPtr l);
[DllImport("user32.dll")] public static extern uint MapVirtualKey(uint c, uint t);
'@

$script:hwnd = [IntPtr]::Zero
$script:shotDir = 'C:\gba\hookedsmith\build\shots'

function Start-Game($rom) {
    New-Item -ItemType Directory -Force $script:shotDir | Out-Null
    Remove-Item "$script:shotDir\*.png" -ErrorAction SilentlyContinue
    $p = Start-Process 'C:\Program Files\mGBA\mGBA.exe' -ArgumentList @('-3', '-C', "screenshotPath=$script:shotDir", $rom) -PassThru
    for ($i = 0; $i -lt 40; $i++) {
        Start-Sleep -Milliseconds 250
        $p.Refresh()
        if ($p.MainWindowHandle -ne [IntPtr]::Zero) { break }
    }
    $script:hwnd = $p.MainWindowHandle
    return $p
}

function Press($vk, $ms = 90) {
    $scan = [MG.Win]::MapVirtualKey([uint32]$vk, 0)
    $down = [IntPtr](1 -bor ($scan -shl 16))
    $up = [IntPtr](1 -bor ($scan -shl 16) -bor 0xC0000000)
    [MG.Win]::PostMessage($script:hwnd, 0x100, [IntPtr]$vk, $down) | Out-Null
    Start-Sleep -Milliseconds $ms
    [MG.Win]::PostMessage($script:hwnd, 0x101, [IntPtr]$vk, $up) | Out-Null
    Start-Sleep -Milliseconds 120
}

$VK = @{ A = 0x58; B = 0x5A; L = 0x41; R = 0x53; START = 0x0D; SELECT = 0x20; LEFT = 0x25; UP = 0x26; RIGHT = 0x27; DOWN = 0x28 }

function Shot($name) {
    $before = @(Get-ChildItem $script:shotDir -Filter *.png -ErrorAction SilentlyContinue).Count
    Press 0x7B 100     # F12
    for ($i = 0; $i -lt 20; $i++) {
        Start-Sleep -Milliseconds 100
        $files = @(Get-ChildItem $script:shotDir -Filter *.png -ErrorAction SilentlyContinue | Sort-Object LastWriteTime)
        if ($files.Count -gt $before) {
            $new = $files[-1]
            Move-Item $new.FullName "$script:shotDir\$name.png" -Force
            return "ok $name"
        }
    }
    return "FAILED $name"
}
