# mGBA 창을 초당 약 20장 녹화하는 도우미 (tools/mgba_auto.ps1을 먼저 불러야 한다: . tools\mgba_auto.ps1; . tools\record_helper.ps1)
# Rec-Run: 정해진 시간 동안 프레임을 캡처하면서, 일정(schedule)에 맞춰 키를 누른다. Rec-Save: 프레임을 PNG로, 시각을 times.txt로 저장한다.
# 일정 한 줄: @{ t = 시작ms; vk = $VK.A; hold = 누르는 ms }  (t는 Rec-Run 시작 기준)
# GIF로 묶기: python -B tools/make_event_gifs.py <폴더>

function Rec-Grab {
    $rect = New-Object MG.Win+RECT
    $point = New-Object MG.Win+POINT
    [void][MG.Win]::GetClientRect($script:hwnd, [ref]$rect)
    [void][MG.Win]::ClientToScreen($script:hwnd, [ref]$point)
    $w = $rect.Right - $rect.Left; $h = $rect.Bottom - $rect.Top
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
    $g.Dispose(); $g2.Dispose(); $bmp.Dispose()
    return $small
}

function Rec-Run($durationMs, $schedule, $intervalMs = 45) {
    $script:recFrames = New-Object System.Collections.ArrayList
    $script:recTimes = New-Object System.Collections.ArrayList
    $sw = [System.Diagnostics.Stopwatch]::StartNew()
    $state = @{}
    foreach ($i in 0..($schedule.Count - 1)) { $state[$i] = 0 }
    $next = 0
    while ($sw.ElapsedMilliseconds -lt $durationMs) {
        $now = $sw.ElapsedMilliseconds
        for ($i = 0; $i -lt $schedule.Count; $i++) {
            $e = $schedule[$i]
            $scan = [MG.Win]::MapVirtualKey([uint32]$e.vk, 0)
            if ($state[$i] -eq 0 -and $now -ge $e.t) {
                [void][MG.Win]::PostMessage($script:hwnd, 0x100, [IntPtr]$e.vk, [IntPtr](1 -bor ($scan -shl 16)))
                $state[$i] = 1
            }
            elseif ($state[$i] -eq 1 -and $now -ge ($e.t + $e.hold)) {
                [void][MG.Win]::PostMessage($script:hwnd, 0x101, [IntPtr]$e.vk, [IntPtr](1 -bor ($scan -shl 16) -bor 0xC0000000))
                $state[$i] = 2
            }
        }
        if ($now -ge $next) {
            [void]$script:recFrames.Add((Rec-Grab))
            [void]$script:recTimes.Add($sw.ElapsedMilliseconds)
            $next += $intervalMs
        }
        else { Start-Sleep -Milliseconds 2 }
    }
}

function Rec-Save($dir) {
    New-Item -ItemType Directory -Force $dir | Out-Null
    Remove-Item "$dir\*.png", "$dir\times.txt" -ErrorAction SilentlyContinue
    for ($i = 0; $i -lt $script:recFrames.Count; $i++) {
        $script:recFrames[$i].Save(("{0}\f{1:d4}.png" -f $dir, $i), [System.Drawing.Imaging.ImageFormat]::Png)
        $script:recFrames[$i].Dispose()
    }
    ($script:recTimes -join "`n") | Set-Content "$dir\times.txt"
    return $script:recFrames.Count
}
