import { useQuery, useQueryClient } from "@tanstack/solid-query";
import { For, Show } from "solid-js";
import { fetchSensorSettings, toggleSensorFunction } from "@/api/client";
import {
  Card,
  CardContent,
  CardHeader,
  CardTitle,
} from "@/components/ui/card";
import {
  Switch,
  SwitchControl,
  SwitchInput,
  SwitchLabel,
  SwitchThumb,
} from "@/components/ui/switch";

export default function SettingsPage() {
  const queryClient = useQueryClient();

  const query = useQuery(() => ({
    queryKey: ["sensorSettings"],
    queryFn: fetchSensorSettings,
  }));

  async function handleToggle(name: string, enabled: boolean) {
    try {
      await toggleSensorFunction(name, enabled);
      queryClient.invalidateQueries({ queryKey: ["sensorSettings"] });
    } catch (e) {
      console.error("Failed to toggle sensor:", e);
    }
  }

  return (
    <div class="max-w-2xl mx-auto space-y-4">
      <h1 class="text-2xl font-bold">Sensor Settings</h1>
      <p class="text-muted-foreground text-sm">
        Enable or disable sensor functions for live data monitoring.
      </p>

      <Show
        when={!query.isPending}
        fallback={<p class="text-muted-foreground">Loading settings...</p>}
      >
        <div class="space-y-4">
          <For each={query.data}>
            {(sensor) => (
              <Card>
                <CardHeader>
                  <CardTitle class="text-base">{sensor.sensorName}</CardTitle>
                </CardHeader>
                <CardContent class="space-y-3">
                  <For each={sensor.functions}>
                    {(fn) => (
                      <Switch
                        class="flex items-center justify-between gap-4"
                        checked={fn.state}
                        onChange={(checked: boolean) =>
                          handleToggle(fn.name, checked)
                        }
                      >
                        <SwitchInput />
                        <SwitchLabel class="flex-1 font-mono text-xs">
                          {fn.name}
                        </SwitchLabel>
                        <SwitchControl>
                          <SwitchThumb />
                        </SwitchControl>
                      </Switch>
                    )}
                  </For>
                </CardContent>
              </Card>
            )}
          </For>
        </div>
      </Show>
    </div>
  );
}
