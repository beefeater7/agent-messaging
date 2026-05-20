# OpenCode Nudges

`oc-nudge` sends an async message into a pinned OpenCode session on another server.

The route is only:

- OpenCode server URL
- OpenCode session ID

The nudge body includes the sender's server URL and session ID so the receiving agent can reply by sending a new nudge back through the same utility.

## Usage

```bash
oc-nudge --from FROM_ALIAS TARGET_ALIAS "message"
```

or without aliases:

```bash
oc-nudge \
  --from-server http://nixos.local:1234 \
  --from-session ses_sender \
  --to-server http://192.168.0.23:2341 \
  --to-session ses_recipient \
  "message"
```

By default, `oc-nudge` posts to `/session/:id/prompt_async` and does not wait for the remote assistant response. Replies should be explicit new nudges.

## Targets

Set `OC_NUDGE_TARGETS` to a target table, or create `targets.local.tsv` next to the script.

Format:

```text
alias  server-url  session-id
```

See `targets.example.tsv`.
