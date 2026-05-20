# OpenCode Async Nudge Test

Date: 2026-05-20

## Command

```bash
/persist/projects/agent-messaging/transports/opencode/oc-nudge \
  --from-server http://nixos.local:1234 \
  --from-session ses_1be0ecdc7ffeprbWm0Q81O0ZYI \
  --to-server http://192.168.0.23:2341 \
  --to-session ses_1be75dd3effeUNSjObL0vNjp0u \
  "Async oc-nudge smoke test from NixOS. No reply required."
```

## Result

`oc-nudge` posted to the async OpenCode endpoint and returned immediately:

```text
Nudge sent to http://192.168.0.23:2341/session/ses_1be75dd3effeUNSjObL0vNjp0u (HTTP 204).
```

## Notes

- The command used `/session/:id/prompt_async`, not the synchronous `/message` endpoint.
- The HTTP response was only transport acknowledgement; it was not treated as the reply channel.
- The nudge body included `From-Server` and `From-Session` so a recipient can reply by sending a new nudge.
