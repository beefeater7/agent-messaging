# Agent Messaging

This project collects small, local-first ways for agents on separate machines to exchange messages.

## Goals

- Keep a stable human-facing interface for sending short agent messages between machines.
- Prefer transports that use software already present on the host.
- Treat each transport as an implementation detail behind a simple message-send abstraction.
- Preserve useful experiments in `/persist` instead of impermanent working directories like `~/Downloads`.
- Make LAN OpenCode API messaging the main path when machines can reach each other over the network.
- Keep Bluetooth object push as a nearby/offline fallback transport.

## Current Transports

- `transports/bluetooth/bt-msg`: sends a text message as a small file over Bluetooth.
- LAN OpenCode API: exploratory; target shape is a named endpoint with a pinned session ID.

## Open Questions

- How should machine aliases map to OpenCode base URLs and session IDs?
- Should session pinning live in a checked-in example config, a private local config, or both?
- What should an agent do when a target OpenCode session is busy?
- Should Bluetooth and LAN messages share one command eventually, or stay as separate tools?
