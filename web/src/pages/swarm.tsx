import { useNavigate } from "@solidjs/router";
import { useQuery } from "@tanstack/solid-query";
import { For, Show } from "solid-js";
import { fetchSwarmData, locateDevice } from "@/api/client";
import { Badge } from "@/components/ui/badge";
import { Button } from "@/components/ui/button";
import {
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableHeader,
  TableRow,
} from "@/components/ui/table";

function formatUptime(ms: number): string {
  const s = Math.floor(ms / 1000);
  const h = Math.floor(s / 3600);
  const m = Math.floor((s % 3600) / 60);
  const sec = s % 60;
  if (h > 0) return `${h}h ${m}m ${sec}s`;
  if (m > 0) return `${m}m ${sec}s`;
  return `${sec}s`;
}

function formatLastSeen(ms: number): string {
  if (ms < 1000) return "just now";
  if (ms < 60000) return `${Math.floor(ms / 1000)}s ago`;
  return `${Math.floor(ms / 60000)}m ago`;
}

export default function SwarmPage() {
  const navigate = useNavigate();

  const query = useQuery(() => ({
    queryKey: ["swarm"],
    queryFn: fetchSwarmData,
    refetchInterval: 1000,
  }));

  const devices = () =>
    [...(query.data ?? [])].sort((a, b) => a.mac.localeCompare(b.mac));

  const onlineCount = () => devices().filter((d) => d.online).length;

  return (
    <div class="max-w-4xl mx-auto space-y-4">
      <div class="flex items-center justify-between">
        <h1 class="text-2xl font-bold">Swarm Devices</h1>
        <span class="text-sm text-muted-foreground">
          {devices().length} devices ({onlineCount()} online)
        </span>
      </div>

      <Show
        when={!query.isPending}
        fallback={<p class="text-muted-foreground">Loading...</p>}
      >
        <Show
          when={devices().length > 0}
          fallback={
            <p class="text-muted-foreground">No devices connected yet.</p>
          }
        >
          <Table>
            <TableHeader>
              <TableRow>
                <TableHead>Status</TableHead>
                <TableHead>MAC Address</TableHead>
                <TableHead>Messages</TableHead>
                <TableHead>Uptime</TableHead>
                <TableHead>Last Seen</TableHead>
                <TableHead class="w-[100px]">Actions</TableHead>
              </TableRow>
            </TableHeader>
            <TableBody>
              <For each={devices()}>
                {(device) => (
                  <TableRow
                    class={
                      device.online
                        ? "cursor-pointer hover:bg-accent"
                        : "opacity-60"
                    }
                    onClick={() => {
                      if (device.online) {
                        navigate(
                          `/livedata?mac=${encodeURIComponent(device.mac)}`,
                        );
                      }
                    }}
                  >
                    <TableCell>
                      <Badge
                        variant={device.online ? "default" : "destructive"}
                        class={
                          device.online
                            ? "bg-success text-success-foreground"
                            : ""
                        }
                      >
                        {device.online ? "Online" : "Offline"}
                      </Badge>
                    </TableCell>
                    <TableCell class="font-mono text-xs">
                      {device.mac}
                    </TableCell>
                    <TableCell>{device.counter}</TableCell>
                    <TableCell>{formatUptime(device.uptime)}</TableCell>
                    <TableCell>{formatLastSeen(device.lastSeen)}</TableCell>
                    <TableCell>
                      <Show when={device.online}>
                        <Button
                          variant="outline"
                          size="sm"
                          onClick={(e: MouseEvent) => {
                            e.stopPropagation();
                            locateDevice(device.mac);
                          }}
                        >
                          Locate
                        </Button>
                      </Show>
                    </TableCell>
                  </TableRow>
                )}
              </For>
            </TableBody>
          </Table>
        </Show>
      </Show>
    </div>
  );
}
