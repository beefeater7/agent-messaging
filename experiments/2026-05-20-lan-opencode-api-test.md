# LAN OpenCode API Test

Date: 2026-05-20

## Target

- Host: `192.168.0.23`
- Port: `2341`
- Session: `ses_1be75dd3effeUNSjObL0vNjp0u`
- Session title observed from API: `Greeting`
- Session directory observed from API: `/home/erikj`

## Request

Endpoint:

```text
POST http://192.168.0.23:2341/session/ses_1be75dd3effeUNSjObL0vNjp0u/message
```

Body:

```json
{
  "parts": [
    {
      "type": "text",
      "text": "LAN OpenCode API test from nixos at 192.168.0.x. Please reply with exactly: agent-messaging API test received"
    }
  ]
}
```

## Result

The remote OpenCode server returned HTTP success and an assistant response in the pinned session.

Response text:

```text
agent-messaging API test received
```

Returned assistant metadata included:

- Message ID: `msg_e45f97d8f00252lV2VU8bs7Tn1`
- Session ID: `ses_1be75dd3effeUNSjObL0vNjp0u`
- Agent: `build`
- Model: `openai/gpt-5.5`
- Finish reason: `stop`

## Takeaways

- A remote OpenCode session can be pinned by including the session ID in the HTTP path.
- The server port identifies the machine/server, not the session.
- A minimal `oc-msg` abstraction can map a target name to `{ base_url, session_id }` and POST a text part to `/session/:id/message`.
