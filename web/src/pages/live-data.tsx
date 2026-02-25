import { useSearchParams } from "@solidjs/router";
import { useQuery } from "@tanstack/solid-query";
import { createSignal, Index, Show } from "solid-js";
import { A } from "@solidjs/router";
import { fetchSensorValues } from "@/api/client";
import { SensorChart } from "@/components/sensor-chart";
import { Badge } from "@/components/ui/badge";
import { Button } from "@/components/ui/button";
import {
  Slider,
  SliderFill,
  SliderLabel,
  SliderThumb,
  SliderTrack,
  SliderValueLabel,
} from "@/components/ui/slider";

export default function LiveDataPage() {
  const [searchParams] = useSearchParams();
  const mac = () => searchParams.mac as string | undefined;
  const isRemote = () => !!mac();
  const [chartLimit, setChartLimit] = createSignal(100);

  const query = useQuery(() => ({
    queryKey: ["sensorValues", mac()],
    queryFn: () => fetchSensorValues(mac()),
    refetchInterval: isRemote() ? 1000 : 100,
  }));

  return (
    <div class="max-w-6xl mx-auto space-y-4">
      <div class="flex items-center justify-between flex-wrap gap-2">
        <div class="flex items-center gap-3">
          <h1 class="text-2xl font-bold">Live Data</h1>
          <Show when={isRemote()}>
            <Badge variant="outline" class="font-mono text-xs">
              {mac()}
            </Badge>
          </Show>
        </div>
        <Show when={isRemote()}>
          <A href="/swarm">
            <Button variant="outline" size="sm">
              Back to Swarm
            </Button>
          </A>
        </Show>
      </div>

      <div class="max-w-sm">
        <Slider
          value={[chartLimit()]}
          onChange={(v: number[]) => setChartLimit(v[0])}
          minValue={50}
          maxValue={2000}
          step={50}
        >
          <div class="flex justify-between w-full">
            <SliderLabel>Chart Data Points</SliderLabel>
            <SliderValueLabel />
          </div>
          <SliderTrack>
            <SliderFill />
            <SliderThumb />
          </SliderTrack>
        </Slider>
      </div>

      <Show
        when={!query.isPending}
        fallback={<p class="text-muted-foreground">Loading sensor data...</p>}
      >
        <Show
          when={(query.data ?? []).length > 0}
          fallback={
            <p class="text-muted-foreground">
              No sensor data available.{" "}
              {!isRemote() && "Enable sensors in Settings."}
            </p>
          }
        >
          <div class="grid gap-4 md:grid-cols-2 xl:grid-cols-3">
            <Index each={query.data}>
              {(sensor) => (
                <div class="rounded-xl border bg-card p-4">
                  <SensorChart
                    name={sensor().name}
                    value={sensor().value}
                    chartLimit={chartLimit()}
                  />
                </div>
              )}
            </Index>
          </div>
        </Show>
      </Show>
    </div>
  );
}
