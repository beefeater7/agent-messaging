# Agent Messaging

This directory is the initial persisted workspace for cross-machine agent interaction experiments.

## Goals

- Keep a stable human-facing interface for sending short agent messages between machines.
- Prefer transports that use software already present on the host.
- Treat each transport as an implementation detail behind a simple message-send abstraction.
- Preserve useful experiments in `/persist` instead of impermanent working directories like `~/Downloads`.
- Investigate LAN OpenCode API messaging as the next promising path when machines can reach each other over the network.
- Keep Bluetooth object push as a nearby/offline fallback transport.

## Current Transports

- `transports/bluetooth/bt-msg`: sends a text message as a small file over Bluetooth.
- `transports/opencode/oc-nudge`: sends an async nudge into a pinned OpenCode session over HTTP. **Standard use:** `oc-nudge TARGET "message"` with no `--from*` flags; sender route comes from `oc-snoop`.
- `transports/opencode/oc-snoop`: auto-discovers local OpenCode server URL and session ID (process tree / listening ports). Used automatically by `oc-nudge` when sender flags are omitted.
- `whoami/oc-whoami`: 100% DB-based session ID discovery by matching its own "running" part in the SQLite database.

## Hostname Policy

Lazy brand-name policy: change hostname to machine brand when convenient.

```
nixos      →  samsung  (Samsung laptop)
erik-...   →  thinkpad (Lenovo ThinkPad)
```

Use `.local` hostnames everywhere — never hardcode LAN IPs. Avahi/mDNS handles resolution automatically when subnets change.

## Open Questions

- How should machine aliases map to OpenCode base URLs and session IDs?
- Should session pinning live in a checked-in example config, a private local config, or both?
- What should an agent do when a target OpenCode session is busy?
- Should Bluetooth and LAN messages share one command eventually, or stay as separate tools?
- Should this workspace remain one project, or split into smaller subprojects as the experiments mature?
