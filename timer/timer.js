import { tool } from "@opencode-ai/plugin"
import { dirname, join } from "node:path"
import { fileURLToPath } from "node:url"

const here = dirname(fileURLToPath(import.meta.url))
const taskPath = join(here, "notify", "task")

async function run(args, cwd) {
  const proc = Bun.spawn(args, {
    cwd,
    stdout: "pipe",
    stderr: "pipe",
  })
  const [stdout, stderr, exitCode] = await Promise.all([
    new Response(proc.stdout).text(),
    new Response(proc.stderr).text(),
    proc.exited,
  ])
  if (exitCode !== 0 && stdout.trim() === "") {
    throw new Error((stderr || `timer exited with status ${exitCode}`).trim())
  }
  return (stdout || stderr).trim()
}

export default tool({
  description: "Start, finish, inspect, or clear the local OpenCode completion timer.",
  args: {
    action: tool.schema.enum(["start", "finish", "status", "clear"]).describe("Timer action to run."),
    label: tool.schema.string().optional().describe("Task label to show in status output."),
    threshold: tool.schema.number().int().min(0).max(86400).optional().describe("Seconds before finish rings the bell. Only used with start."),
  },
  async execute(args, context) {
    const command = [taskPath, args.action]
    if (args.action === "start" && args.threshold !== undefined) {
      command.push("--threshold", String(args.threshold))
    }
    if (args.action === "start" && args.label) {
      command.push(args.label)
    }
    return await run(command, context.directory)
  },
})
