export interface SwarmDevice {
  mac: string;
  counter: number;
  uptime: number;
  lastSeen: number;
  online: boolean;
  powerMw: number;
}

export interface SensorValue {
  name: string;
  value: string;
}

export interface LogEntry {
  level: string;
  timestamp: string;
  message: string;
  mac?: string;
}

export interface SensorFunction {
  name: string;
  state: boolean;
}

export interface SensorGroup {
  sensorName: string;
  functions: SensorFunction[];
}

export type LogLevel = "ALL" | "INFO" | "WARNING" | "ERROR" | "DEBUG" | "TRACE";

export async function fetchSwarmData(): Promise<SwarmDevice[]> {
  const res = await fetch("/getSwarmData");
  if (!res.ok) throw new Error("Failed to fetch swarm data");
  return res.json();
}

export async function fetchSensorValues(mac?: string): Promise<SensorValue[]> {
  const url = mac
    ? `/getEnabledSensorValues?mac=${encodeURIComponent(mac)}`
    : "/getEnabledSensorValues";
  const res = await fetch(url);
  if (!res.ok) throw new Error("Failed to fetch sensor values");
  return res.json();
}

function buildLogsQuery(level: LogLevel, mac?: string): string {
  const params = new URLSearchParams({ level });
  if (mac) {
    params.set("mac", mac);
  }
  return params.toString();
}

export async function fetchLogs(level: LogLevel, mac?: string): Promise<LogEntry[]> {
  const res = await fetch(`/logging/getLogs?${buildLogsQuery(level, mac)}`);
  if (!res.ok) throw new Error("Failed to fetch logs");
  return res.json();
}

export async function fetchNewLogs(level: LogLevel, mac?: string): Promise<LogEntry[]> {
  const res = await fetch(`/logging/getNewLogs?${buildLogsQuery(level, mac)}`);
  if (!res.ok) throw new Error("Failed to fetch new logs");
  return res.json();
}

export async function fetchSensorSettings(): Promise<SensorGroup[]> {
  const res = await fetch("/settings/getSensorData");
  if (!res.ok) throw new Error("Failed to fetch sensor settings");
  return res.json();
}

export async function locateDevice(mac: string): Promise<void> {
  const res = await fetch("/command/locate", {
    method: "POST",
    headers: { "Content-Type": "application/x-www-form-urlencoded" },
    body: `mac=${encodeURIComponent(mac)}`,
  });
  if (!res.ok) throw new Error("Failed to send locate command");
}

export async function forwardDevice(mac: string): Promise<void> {
  const res = await fetch("/command/forward", {
    method: "POST",
    headers: { "Content-Type": "application/x-www-form-urlencoded" },
    body: `mac=${encodeURIComponent(mac)}`,
  });
  if (!res.ok) throw new Error("Failed to send forward command");
}

export async function stopDevice(mac: string): Promise<void> {
  const res = await fetch("/command/stop", {
    method: "POST",
    headers: { "Content-Type": "application/x-www-form-urlencoded" },
    body: `mac=${encodeURIComponent(mac)}`,
  });
  if (!res.ok) throw new Error("Failed to send stop command");
}

export async function toggleSensorFunction(
  sensorFunction: string,
  enabled: boolean,
): Promise<void> {
  const res = await fetch("/settings/toggleFunction", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ sensorFunction, enabled }),
  });
  if (!res.ok) throw new Error("Failed to toggle sensor function");
}
