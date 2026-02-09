import { useQuery } from "@tanstack/solid-query";
import { createSignal, For, Show, createEffect, on } from "solid-js";
import { fetchLogs, fetchNewLogs, type LogEntry, type LogLevel } from "@/api/client";
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

const levelColors: Record<string, string> = {
  INFO: "bg-info text-info-foreground",
  WARNING: "bg-warning text-warning-foreground",
  ERROR: "bg-destructive text-white",
  DEBUG: "bg-secondary text-secondary-foreground",
  TRACE: "bg-muted text-muted-foreground",
};

export default function LoggingPage() {
  const [level, setLevel] = createSignal<LogLevel>("ALL");
  const [allLogs, setAllLogs] = createSignal<LogEntry[]>([]);

  const initialQuery = useQuery(() => ({
    queryKey: ["logs", level()],
    queryFn: () => fetchLogs(level()),
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
    queryKey: ["newLogs", level()],
    queryFn: () => fetchNewLogs(level()),
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
          <Button variant="outline" size="sm" onClick={handleRefresh}>
            Refresh
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
