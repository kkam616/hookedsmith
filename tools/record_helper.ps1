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

# Rec-Auto: 창(이벤트)이 뜨는 것을 화면에서 찾아 그 시각을 기준으로 키를 누르며 녹화한다.
#  $pre: Rec-Run과 같은 키 일정 (이벤트를 일으키는 준비 동작)
#  $repeatVk / $repeatFrom / $repeatEvery: 준비 후 이벤트가 뜰 때까지 이 키를 주기적으로 누른다 (없으면 $null)
#  $afterDelays: 이벤트가 뜬 뒤 A를 누를 시각(ms) 목록, $tailMs: 마지막 키 뒤 더 녹화할 시간
#  결과: $script:recStart = 이벤트가 뜬 시각(ms, 녹화 시작 기준)
function Rec-Auto($pre, $repeatVk, $repeatFrom, $repeatEvery, $afterDelays, $tailMs, $maxMs = 90000) {
    $script:recFrames = New-Object System.Collections.ArrayList
    $script:recTimes = New-Object System.Collections.ArrayList
    $sw = [System.Diagnostics.Stopwatch]::StartNew()
    $state = @{}
    foreach ($i in 0..($pre.Count - 1)) { $state[$i] = 0 }
    $next = 0; $nextRepeat = $repeatFrom; $t0 = -1; $afterDone = 0
    $script:recStart = -1
    while ($sw.ElapsedMilliseconds -lt $maxMs) {
        $now = $sw.ElapsedMilliseconds
        for ($i = 0; $i -lt $pre.Count; $i++) {
            $e = $pre[$i]
            $scan = [MG.Win]::MapVirtualKey([uint32]$e.vk, 0)
            if ($state[$i] -eq 0 -and $now -ge $e.t) { [void][MG.Win]::PostMessage($script:hwnd, 0x100, [IntPtr]$e.vk, [IntPtr](1 -bor ($scan -shl 16))); $state[$i] = 1 }
            elseif ($state[$i] -eq 1 -and $now -ge ($e.t + $e.hold)) { [void][MG.Win]::PostMessage($script:hwnd, 0x101, [IntPtr]$e.vk, [IntPtr](1 -bor ($scan -shl 16) -bor 0xC0000000)); $state[$i] = 2 }
        }
        if ($t0 -lt 0 -and $repeatVk -and $now -ge $nextRepeat) {
            $scan = [MG.Win]::MapVirtualKey([uint32]$repeatVk, 0)
            [void][MG.Win]::PostMessage($script:hwnd, 0x100, [IntPtr]$repeatVk, [IntPtr](1 -bor ($scan -shl 16)))
            Start-Sleep -Milliseconds 120
            [void][MG.Win]::PostMessage($script:hwnd, 0x101, [IntPtr]$repeatVk, [IntPtr](1 -bor ($scan -shl 16) -bor 0xC0000000))
            $nextRepeat = $now + $repeatEvery
        }
        if ($t0 -ge 0 -and $afterDone -lt $afterDelays.Count -and ($now - $t0) -ge $afterDelays[$afterDone]) {
            $a = [MG.Win]::MapVirtualKey([uint32]$VK.A, 0)
            [void][MG.Win]::PostMessage($script:hwnd, 0x100, [IntPtr]$VK.A, [IntPtr](1 -bor ($a -shl 16)))
            Start-Sleep -Milliseconds 120
            [void][MG.Win]::PostMessage($script:hwnd, 0x101, [IntPtr]$VK.A, [IntPtr](1 -bor ($a -shl 16) -bor 0xC0000000))
            $afterDone++
            if ($afterDone -eq $afterDelays.Count) { $endAt = $now + $tailMs }
        }
        if ($now -ge $next) {
            $bmp = Rec-Grab
            [void]$script:recFrames.Add($bmp)
            [void]$script:recTimes.Add($sw.ElapsedMilliseconds)
            $next += 45
            if ($t0 -lt 0 -and $now -gt 1500) {
                $l = $bmp.GetPixel(78, 75); $m = $bmp.GetPixel(82, 75); $r = $bmp.GetPixel(161, 75)
                $tan = { param($c) $c.R -gt 150 -and $c.G -gt 110 -and $c.B -lt 140 -and $c.R -gt $c.B + 40 }
                if ((& $tan $l) -and (& $tan $r)) { $t0 = $now; $script:recStart = $now }
            }
        }
        else { Start-Sleep -Milliseconds 2 }
        if ($endAt -and $now -ge $endAt) { break }
    }
}
