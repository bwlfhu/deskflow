# Windows screenshot screen-switch repair

**Goal:** prevent Windows screenshot-related cursor ping-pong while preserving normal hardware mouse input, the CRLF correction and the user's 10 MiB setting. The user approved implementation after the prior investigation and repair proposal.

**Design:** use the bounds of the monitor containing the parking point for the existing bogus-motion guard; synchronize both flushed and queued warps with their measured position; prevent departure when another process captures the pointer or clipping excludes the parking point. Preserve the clipboard protocol and injected-input policy. Do not add a fixed mouse-speed cap or change switching delay.

**Evidence and limits:** this host has a 3840x1080 Windows desktop with a 1920x1080 primary and parking point (960,540); its logs contained 33 switches in 490 ms. Upstream #10030 demonstrates stale warp events accepted by virtual-desktop thresholds; #8198 reports screenshot-related bouncing with Windows/Mac. A direct Feishu/Weixin trace remains unobserved; tests prove corrected mechanisms, not elimination of every intermittent symptom.

- [x] Reproduce old behavior using actual production methods in an isolated event/Win32 harness, with no live hooks or clipboard access.
- [x] Correct parking-monitor geometry, capture/clipping checks and actual-position warp bookkeeping, including failed reads and queue quit/error handling.
- [x] Register persistent production-method event regressions in CTest, using actual message constants and range-filtered fake queues.
- [x] Verify the old guard fails the persistent fixture in an isolated source copy; the repaired fixture passes. Verify translated/negative-coordinate monitors, threshold boundaries and legitimate fast motion.
- [x] Build core and three targeted test executables with MSVC; run all three, including the existing CRLF converter regression. Independent final review found no blocking P1/P2 issue.
- [ ] Stage the new core against the exact installed runtime, verify version/imports/hashes and preserve the GUI, daemon and DLLs. A full UI translation rebuild is unnecessary for this core-only change.
- [ ] Save source patch/bundle, test evidence, manifest and an administrator deployment script with rollback. Back up the current installation and configuration before replacement.
- [ ] Apply the verified package and confirm installed hashes, service health and Mac reconnection. Report that real-world screenshot verification is still needed; do not restart CRLF monitoring.

Verified 2026-10-30: original extracted methods fail 10 of 24 checks, repaired methods pass all 24. The repository CTest fixture passes 26 checks and fails 2 when the old production geometry guard is restored. Geometry policy passes 53,826 checks. MSVC targeted CTest: 3/3 passed. Native clipboard-mutating tests are excluded from this live desktop.
