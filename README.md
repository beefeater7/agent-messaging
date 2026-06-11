# agent-messaging

Cross-machine agent correspondence tools for [OpenCode](https://opencode.ai).

Send async messages between OpenCode sessions over HTTP. Discover machines
on the LAN without manual host configuration. Fall back to Bluetooth when
the network is down.

## Install

```bash
git clone https://github.com/beefeater7/agent-messaging.git
cd agent-messaging
./install.sh      # symlinks tools to ~/.local/bin
```

Requires: `bash`, `curl`, and two machines running OpenCode servers on the
same LAN with Avahi/mDNS enabled.

## Quickstart

```bash
# From one machine, nudge a session on another:
oc-nudge ses_abc123 "hello"

# The target session ID resolves automatically via LAN scan.
# No manual host configuration needed.
```

## Tools

| Tool | What it does |
|---|---|
| `oc-nudge` | Send async messages between OpenCode sessions |
| `oc-find` | Resolve session ID to hostname (LAN scan, auto-cached) |
| `oc-snoop` | Discover local OpenCode server URL (process tree) |
| `oc-whoami` | Discover local session ID (SQLite database) |
| `oc-revive` | Restart local OpenCode server gracefully |
| `bt-msg` | Send text over Bluetooth (offline fallback) |

## How it works

`oc-nudge` POSTs a text message to the target session's `/prompt_async`
endpoint over HTTP. The sender's identity (server + session) is included
in the message body so the recipient can reply.

`oc-find` resolves session IDs to `.local` hostnames. On first use it
scans the LAN and caches results to a local `hosts` file. Subsequent
lookups hit the cache — no rescan.

## Multi-machine setup

1. Run OpenCode with `--hostname 0.0.0.0 --port 4096` on each machine
2. Enable Avahi/mDNS so `.local` hostnames resolve
3. Clone this repo on each machine, run `install.sh`
4. That's it — `oc-nudge ses_target "msg"` works immediately

No configuration files, no static IPs, no port forwarding.
