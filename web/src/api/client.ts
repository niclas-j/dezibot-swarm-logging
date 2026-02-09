export interface SwarmDevice {
  mac: string;
  counter: number;
  uptime: number;
  lastSeen: number;
  online: boolean;
}

export interface SensorValue {
  name: string;
  value: string;
}

export interface LogEntry {
  level: string;
  timestamp: string;
  message: string;
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

export async function fetchLogs(level: LogLevel): Promise<LogEntry[]> {
  const res = await fetch(`/logging/getLogs?level=${level}`);
  if (!res.ok) throw new Error("Failed to fetch logs");
  return res.json();
}

export async function fetchNewLogs(level: LogLevel): Promise<LogEntry[]> {
  const res = await fetch(`/logging/getNewLogs?level=${level}`);
  if (!res.ok) throw new Error("Failed to fetch new logs");
  return res.json();
}

export async function fetchSensorSettings(): Promise<SensorGroup[]> {
  const res = await fetch("/settings/getSensorData");
  if (!res.ok) throw new Error("Failed to fetch sensor settings");
  return res.json();
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
