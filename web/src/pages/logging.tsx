import { useNavigate, useSearchParams } from "@solidjs/router";
import { useQuery } from "@tanstack/solid-query";
import { createSignal, For, Show, createEffect, on } from "solid-js";
import {
  fetchLogs,
  fetchNewLogs,
  type LogEntry,
  type LogLevel,
} from "@/api/client";
import { Badge } from "@/components/ui/badge";
import { Button } from "@/components/ui/button";
import {
  Select,
  SelectContent,
  SelectItem,
  SelectTrigger,
  SelectValue,
} from "@/components/ui/select";
import {
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableHeader,
  TableRow,
} from "@/components/ui/table";

const LOG_LEVELS: LogLevel[] = [
  "ALL",
  "INFO",
  "WARNING",
  "ERROR",
  "DEBUG",
  "TRACE",
];

const ALL_SENDERS = "ALL";

const levelColors: Record<string, string> = {
  INFO: "bg-info text-info-foreground",
  WARNING: "bg-warning text-warning-foreground",
  ERROR: "bg-destructive text-white",
  DEBUG: "bg-secondary text-secondary-foreground",
  TRACE: "bg-muted text-muted-foreground",
};

function escapeCsvCell(value: string): string {
  const escaped = value.replace(/"/g, '""');
  return /[",\r\n]/.test(escaped) ? `"${escaped}"` : escaped;
}

function buildCsv(logs: LogEntry[]): string {
  const header = ["timestamp", "level", "mac", "message"];
  const rows = logs.map((log) => [
    log.timestamp,
    log.level,
    log.mac ?? "",
    log.message,
  ]);

  return [header, ...rows]
    .map((row) => row.map((cell) => escapeCsvCell(cell)).join(","))
    .join("\r\n");
}

function buildExportFilename(level: LogLevel, sender: string): string {
  const now = new Date();
  const pad = (n: number) => n.toString().padStart(2, "0");
  const timestamp = `${now.getFullYear()}${pad(now.getMonth() + 1)}${pad(now.getDate())}_${pad(now.getHours())}${pad(now.getMinutes())}${pad(now.getSeconds())}`;
  const senderToken = sender === ALL_SENDERS ? "all-senders" : sender.replace(/:/g, "-");
  return `logs_${level.toLowerCase()}_${senderToken}_${timestamp}.csv`;
}

export default function LoggingPage() {
  const navigate = useNavigate();
  const [searchParams] = useSearchParams();
  const [level, setLevel] = createSignal<LogLevel>("ALL");
  const [sender, setSender] = createSignal(
    typeof searchParams.mac === "string" && searchParams.mac.length > 0
      ? searchParams.mac
      : ALL_SENDERS,
  );
  const [allLogs, setAllLogs] = createSignal<LogEntry[]>([]);
  const [isExporting, setIsExporting] = createSignal(false);

  const selectedMac = () =>
    sender() === ALL_SENDERS ? undefined : (sender() as string);

  const senderOptions = () => {
    const senders = new Set(
      allLogs()
        .map((log) => log.mac)
        .filter((mac): mac is string => !!mac && mac.length > 0),
    );
    if (sender() !== ALL_SENDERS) {
      senders.add(sender());
    }

    return [ALL_SENDERS, ...Array.from(senders).sort((a, b) => a.localeCompare(b))];
  };

  createEffect(() => {
    const macParam = searchParams.mac;
    const nextSender =
      typeof macParam === "string" && macParam.length > 0
        ? macParam
        : ALL_SENDERS;

    if (nextSender !== sender()) {
      setSender(nextSender);
      setAllLogs([]);
    }
  });

  const initialQuery = useQuery(() => ({
    queryKey: ["logs", level(), sender()],
    queryFn: () => fetchLogs(level(), selectedMac()),
    refetchOnWindowFocus: false,
  }));

  createEffect(
    on(
      () => initialQuery.data,
      (data) => {
        if (data) setAllLogs(data);
      },
    ),
  );

  const newLogsQuery = useQuery(() => ({
    queryKey: ["newLogs", level(), sender()],
    queryFn: () => fetchNewLogs(level(), selectedMac()),
    refetchInterval: 1000,
    refetchOnWindowFocus: false,
  }));

  createEffect(
    on(
      () => newLogsQuery.data,
      (newData) => {
        if (newData && newData.length > 0) {
          setAllLogs((prev) => [...prev, ...newData]);
        }
      },
    ),
  );

  function handleRefresh() {
    setAllLogs([]);
    initialQuery.refetch();
  }

  function handleLevelChange(value: LogLevel | null) {
    if (value) {
      setLevel(value);
      setAllLogs([]);
    }
  }

  function handleSenderChange(value: string | null) {
    if (!value) {
      return;
    }

    setSender(value);
    setAllLogs([]);

    if (value === ALL_SENDERS) {
      navigate("/logging", { replace: true });
      return;
    }

    navigate(`/logging?mac=${encodeURIComponent(value)}`, { replace: true });
  }

  async function handleExportCsv() {
    if (isExporting()) {
      return;
    }

    setIsExporting(true);
    try {
      const logs = await fetchLogs(level(), selectedMac());
      const csvContent = buildCsv(logs);
      const blob = new Blob([csvContent], { type: "text/csv;charset=utf-8" });
      const url = URL.createObjectURL(blob);
      const link = document.createElement("a");
      link.href = url;
      link.download = buildExportFilename(level(), sender());
      document.body.appendChild(link);
      link.click();
      link.remove();
      URL.revokeObjectURL(url);
    } catch (error) {
      console.error(error);
      window.alert("Failed to export logs as CSV.");
    } finally {
      setIsExporting(false);
    }
  }

  return (
    <div class="max-w-5xl mx-auto space-y-4">
      <div class="flex items-center justify-between flex-wrap gap-2">
        <h1 class="text-2xl font-bold">Logs</h1>
        <div class="flex items-center gap-2">
          <Select<LogLevel>
            value={level()}
            onChange={handleLevelChange}
            options={LOG_LEVELS}
            itemComponent={(props) => (
              <SelectItem item={props.item}>{props.item.rawValue}</SelectItem>
            )}
          >
            <SelectTrigger>
              <SelectValue<LogLevel>>
                {(state) => state.selectedOption()}
              </SelectValue>
            </SelectTrigger>
            <SelectContent />
          </Select>
          <Select<string>
            value={sender()}
            onChange={handleSenderChange}
            options={senderOptions()}
            itemComponent={(props) => (
              <SelectItem item={props.item}>{props.item.rawValue}</SelectItem>
            )}
          >
            <SelectTrigger>
              <SelectValue<string>>
                {(state) =>
                  state.selectedOption() === ALL_SENDERS
                    ? "All senders"
                    : state.selectedOption()
                }
              </SelectValue>
            </SelectTrigger>
            <SelectContent />
          </Select>
          <Button variant="outline" size="sm" onClick={handleRefresh}>
            Refresh
          </Button>
          <Button
            variant="outline"
            size="sm"
            onClick={handleExportCsv}
            disabled={initialQuery.isPending || isExporting()}
          >
            {isExporting() ? "Exporting..." : "Export CSV"}
          </Button>
        </div>
      </div>

      <Show
        when={!initialQuery.isPending}
        fallback={<p class="text-muted-foreground">Loading logs...</p>}
      >
        <Show
          when={allLogs().length > 0}
          fallback={<p class="text-muted-foreground">No log entries.</p>}
        >
          <div class="max-h-[70vh] overflow-auto rounded-xl border">
            <Table>
              <TableHeader>
                <TableRow>
                  <TableHead class="w-[90px]">Level</TableHead>
                  <TableHead class="w-[180px]">Timestamp</TableHead>
                  <TableHead class="w-[160px]">Sender</TableHead>
                  <TableHead>Message</TableHead>
                </TableRow>
              </TableHeader>
              <TableBody>
                <For each={allLogs()}>
                  {(log) => (
                    <TableRow>
                      <TableCell>
                        <Badge
                          class={
                            levelColors[log.level] ??
                            "bg-secondary text-secondary-foreground"
                          }
                        >
                          {log.level}
                        </Badge>
                      </TableCell>
                      <TableCell class="font-mono text-xs">
                        {log.timestamp}
                      </TableCell>
                      <TableCell class="font-mono text-xs">
                        {log.mac && log.mac.length > 0 ? log.mac : "-"}
                      </TableCell>
                      <TableCell class="text-sm">{log.message}</TableCell>
                    </TableRow>
                  )}
                </For>
              </TableBody>
            </Table>
          </div>
        </Show>
      </Show>
    </div>
  );
}
