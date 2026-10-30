# Clipboard CRLF repair and local deployment

Goal: repair the confirmed line-ending conversion defect in Deskflow 1.27.0 and deploy the Windows build for everyday observation, as requested by the user. No controlled sharing-on/off attribution experiment is required.

Design: normalize CRLF as one newline and standalone CR as one newline before generating native text. Internal text uses LF. Preserve existing Windows CRLF and legacy Mac CR output conventions, all intentional blank lines, Unicode and trailing newlines. Do not modify clipboard ownership or protocol behavior in this patch.

Base: official v1.27.0, commit 081f647. Work in an isolated local worktree. Keep original binaries, settings and service configuration for rollback. Creating a GitHub fork requires working account authentication; do not represent a local branch as a published fork.

- [x] Extract original converter functions and run regression expectations: original CRLF normalization fails at runtime.
- [ ] Add native converter regression tests for LF, CRLF, CR, mixed endings, intentional blank lines, trailing endings, Unicode and repeated round trips.
- [ ] Normalize the Windows and Mac converters without changing the public API or clipboard formats.
- [ ] Run portable extracted-function checks and the native Windows converter tests.
- [ ] Build the Windows release using MSVC, Qt 6 and OpenSSL; run the available project tests and review the diff.
- [ ] Commit the focused repair; create a GitHub fork and push if authenticated access is available. Otherwise retain a complete local Git bundle and patch.
- [ ] Stage the built binary and a rollback copy; verify version, runtime dependencies and hashes before touching the running installation.
- [ ] Back up the installed core and configuration, stop the service briefly, replace the core, restart and confirm the Mac reconnects. Preserve existing sharing settings.
- [ ] Record the deployed revision, tests, rollback procedure and any Windows/Mac deployment limitations.

Acceptance: converter tests preserve normalized newline structure, legitimate consecutive blank lines survive, repeated conversions do not grow text, the installed Windows core reports the custom revision and reconnects to the existing Mac. Long-term elimination of the intermittent clipboard incident remains to be observed in normal use.
