# Clipboard CRLF repair and local deployment

Goal: repair the confirmed line-ending conversion defect in Deskflow 1.27.0 and deploy the Windows build for everyday observation, as requested by the user. No controlled sharing-on/off attribution experiment is required.

Design: normalize CRLF as one newline and standalone CR as one newline before generating native text. Internal text uses LF. Preserve existing Windows CRLF and legacy Mac CR output conventions, all intentional blank lines, Unicode and trailing newlines. Do not modify clipboard ownership or protocol behavior in this patch.

Base: official v1.27.0, commit 081f647. Work in an isolated local worktree. Keep original binaries, settings and service configuration for rollback. Creating a GitHub fork requires working account authentication; do not represent a local branch as a published fork.

- [x] Extract original converter functions and run regression expectations: original CRLF normalization fails at runtime.
- [x] Add native converter regression tests for LF, CRLF, CR, mixed endings, intentional blank lines, trailing endings, Unicode and repeated round trips.
- [x] Normalize the Windows and Mac converters without changing the public API or clipboard formats.
- [x] Run portable extracted-function checks and the native Windows converter tests. Portable checks: 11 cases, 11044 assertions. Native tests passed in Windows CI.
- [x] Build the Windows release using MSVC, Qt 6 and OpenSSL; run the available project tests and review the diff. GitHub run 41002248885 completed successfully.
- [x] Commit the focused repair; create GitHub fork bwlfhu/deskflow and publish fix/clipboard-crlf through the authenticated browser. Local Git bundle and patch retained.
- [x] Stage the built binary and a rollback copy; verify version, runtime dependencies and hashes before touching the running installation.
- [x] Back up the installation and configuration, stop the service briefly, replace the installation, restart and confirm the Mac reconnects. Sharing remains enabled.
- [x] Record the deployed revision 1576cfc38de3f473fa5efea0ccfc14c9fdac4d25, tests, rollback procedure and Windows/Mac deployment limitations.

Deployed at 2026-10-30 12:36 Asia/Shanghai. Original directory retained at C:\Program Files\Deskflow.before-crlf-20261030-123624. All 68 installed files match the verified artifact manifest. The initial offscreen Qt version probe was invalid for the distributed plugins; a clean Windows environment probe succeeds. Mac code is repaired in source but its installed application has not been updated because SSH was unavailable.

Acceptance: converter tests preserve normalized newline structure, legitimate consecutive blank lines survive, repeated conversions do not grow text, the installed Windows core reports the custom revision and reconnects to the existing Mac. Long-term elimination of the intermittent clipboard incident remains to be observed in normal use.
