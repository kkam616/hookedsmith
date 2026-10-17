# mGBA 자동 조작 도우미: 창에 키 메시지를 직접 보내고(PostMessage) F12로 스크린샷을 찍는다.
Add-Type -AssemblyName System.Drawing
Add-Type -Namespace MG -Name Win -MemberDefinition @'
[DllImport("user32.dll")] public static extern bool PostMessage(System.IntPtr h, uint m, System.IntPtr w, System.IntPtr l);
[DllImport("user32.dll")] public static extern uint MapVirtualKey(uint c, uint t);
[StructLayout(LayoutKind.Sequential)] public struct RECT { public int Left, Top, Right, Bottom; }
[StructLayout(LayoutKind.Sequential)] public struct POINT { public int X, Y; }
[DllImport("user32.dll")] public static extern bool GetClientRect(System.IntPtr h, out RECT r);
[DllImport("user32.dll")] public static extern bool ClientToScreen(System.IntPtr h, ref POINT p);
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
    # F12 스크린샷이 저장되지 않는 환경이면(가끔 그렇다) 창의 화면 영역을 직접 캡처해 240x160으로 줄여 저장한다.
    $rect = New-Object MG.Win+RECT
    $point = New-Object MG.Win+POINT
    if ([MG.Win]::GetClientRect($script:hwnd, [ref]$rect) -and [MG.Win]::ClientToScreen($script:hwnd, [ref]$point)) {
        $w = $rect.Right - $rect.Left; $h = $rect.Bottom - $rect.Top
        if ($w -gt 0 -and $h -gt 0) {
            # 클라이언트 영역 위쪽에 메뉴 막대가 포함되므로, 게임 화면(가로:세로 = 3:2)만큼 아래쪽을 자른다.
            $video_h = [int]($w * 2 / 3)
            $top = [Math]::Max(0, $h - $video_h)
            $bmp = New-Object System.Drawing.Bitmap $w, $video_h
            $g = [System.Drawing.Graphics]::FromImage($bmp)
            $g.CopyFromScreen($point.X, $point.Y + $top, 0, 0, $bmp.Size)
            $small = New-Object System.Drawing.Bitmap 240, 160
            $g2 = [System.Drawing.Graphics]::FromImage($small)
            $g2.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::NearestNeighbor
            $g2.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::Half
            $g2.DrawImage($bmp, 0, 0, 240, 160)
            $small.Save("$script:shotDir\$name.png", [System.Drawing.Imaging.ImageFormat]::Png)
            $g.Dispose(); $g2.Dispose(); $bmp.Dispose(); $small.Dispose()
            return "ok(screen) $name"
        }
    }
    return "FAILED $name"
}
