# OpenCode Nudges

`oc-nudge` sends an async message into a pinned OpenCode session on another server.

The route is only:

- OpenCode server URL
- OpenCode session ID

The nudge body includes the sender's server URL and session ID so the receiving agent can reply by sending a new nudge back through the same utility.

## Usage

**Standard (recommended):** omit sender flags. With `oc-snoop` on `PATH`, `oc-nudge` discovers this host's OpenCode server and session automatically.

```bash
oc-nudge thinkpad "message"
oc-nudge --model openai/gpt-5.5 thinkpad "use this model on the recipient"
```

**Explicit recipient** (same machine or no target alias):

```bash
oc-nudge --to-server http://127.0.0.1:4096 --to-session ses_recipient "message"
```

**Override sender** when auto-discovery is wrong or unavailable:

```bash
oc-nudge --from samsung thinkpad "message"
```

```bash
oc-nudge \
  --from-server http://samsung.local \
  --from-session ses_sender \
  --to-server http://thinkpad.local \
  --to-session ses_recipient \
  "message"
```

By default, `oc-nudge` posts to `/session/:id/prompt_async` and does not wait for the remote assistant response. Replies should be explicit new nudges (do not poll `/session/.../message` for the other side's answer).

## Targets

Set `OC_NUDGE_TARGETS` to a target table, or create `targets.local.tsv` next to the script.

Format:

```text
alias  server-url  session-id
```

See `targets.example.tsv`.
