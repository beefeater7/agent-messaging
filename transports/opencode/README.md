# OpenCode Nudges

`oc-nudge` sends an async message into a pinned OpenCode session on another server.

The route is only:

- OpenCode server URL
- OpenCode session ID

The nudge body includes the sender's server URL and session ID so the receiving agent can reply by sending a new nudge back through the same utility.

## Usage

**Standard agent use (recommended):** omit sender flags. With `oc-snoop` and `oc-whoami` on `PATH`, `oc-nudge` discovers this host's OpenCode server and the current OpenCode tool session automatically. This requires running as an OpenCode tool call.

```bash
oc-nudge thinkpad "message"
oc-nudge --model openai/gpt-5.5 thinkpad "use this model on the recipient"
```

**Human terminal use:** pass `-p` or `--person` so the sender is labeled `Human` instead of an OpenCode session ID.

```bash
oc-nudge -p thinkpad "message from terminal"
oc-nudge -p thinkpad ses_recipient "message to a specific session"
```

For multi-line stdin input, agents should use an envelope-looking heredoc delimiter. `oc-nudge` trims leading empty lines from stdin so the received nudge envelope owns the spacing.

This form uses an unquoted heredoc delimiter, so shell expansion still applies inside the body. Quote or escape dollar signs, backticks, and command substitutions when they should be sent literally.

```bash
oc-nudge -p thinkpad ses_recipient <<\
────────────────

multi-line message

────────────────
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
