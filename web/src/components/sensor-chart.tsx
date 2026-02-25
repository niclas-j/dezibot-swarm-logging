import { onMount, onCleanup, createSignal, createEffect } from "solid-js";
import {
  Chart,
  LineController,
  LineElement,
  PointElement,
  LinearScale,
  CategoryScale,
  Legend,
  Tooltip,
} from "chart.js";

Chart.register(
  LineController,
  LineElement,
  PointElement,
  LinearScale,
  CategoryScale,
  Legend,
  Tooltip,
);

interface SensorChartProps {
  name: string;
  value: string;
  chartLimit: number;
}

const COLORS = [
  "rgb(54, 162, 235)",
  "rgb(255, 99, 132)",
  "rgb(75, 192, 192)",
];

function parseValue(value: string): { values: number[]; labels: string[] } {
  if (value.includes(",")) {
    const parts = value.split(",").map((p) => p.trim());
    const values: number[] = [];
    const labels: string[] = [];
    for (const part of parts) {
      const colonIdx = part.indexOf(":");
      if (colonIdx === -1) continue;
      const label = part.slice(0, colonIdx).trim();
      const num = parseFloat(part.slice(colonIdx + 1).trim());
      if (!isNaN(num)) {
        values.push(num);
        labels.push(label);
      }
    }
    if (values.length > 0) return { values, labels };
  }

  const numeric = parseFloat(value);
  if (!isNaN(numeric)) {
    return { values: [numeric], labels: ["value"] };
  }

  return { values: [], labels: [] };
}

export function SensorChart(props: SensorChartProps) {
  let canvasRef!: HTMLCanvasElement;
  let chart: Chart | undefined;
  let xVal = 0;
  const dataStore: { x: number; y: number }[][] = [];
  const [ready, setReady] = createSignal(false);

  const displayValue = () => {
    if (props.name.includes("TiltDirection")) {
      const dirMap: Record<string, string> = {
        "0": "Front",
        "1": "Left",
        "2": "Right",
        "3": "Back",
        "4": "Neutral",
        "5": "Flipped",
        "6": "Error",
      };
      return dirMap[props.value] || "Unknown";
    }
    return props.value;
  };

  function addDataPoint(value: string, limit: number) {
    if (!chart) return;
    const { values } = parseValue(value);
    if (values.length === 0) return;
    xVal++;
    values.forEach((v, i) => {
      if (!dataStore[i]) {
        dataStore[i] = [];
        chart!.data.datasets.push({
          label: `series-${i}`,
          data: dataStore[i],
          borderColor: COLORS[i % COLORS.length],
          borderWidth: 1.5,
          pointRadius: 0,
          tension: 0.1,
        });
      }
      dataStore[i].push({ x: xVal, y: v });
      while (dataStore[i].length > limit) {
        dataStore[i].shift();
      }
    });
    chart.update("none");
  }

  onMount(() => {
    const { labels } = parseValue(props.value);
    const datasets = labels.map((label, i) => {
      const data: { x: number; y: number }[] = [];
      dataStore[i] = data;
      return {
        label,
        data,
        borderColor: COLORS[i % COLORS.length],
        borderWidth: 1.5,
        pointRadius: 0,
        tension: 0.1,
      };
    });

    chart = new Chart(canvasRef, {
      type: "line",
      data: { datasets },
      options: {
        responsive: true,
        maintainAspectRatio: false,
        animation: false,
        scales: {
          x: {
            type: "linear",
            display: true,
            ticks: { maxTicksLimit: 6, font: { size: 10 } },
          },
          y: {
            display: true,
            ticks: { font: { size: 10 } },
          },
        },
        plugins: {
          legend: {
            display: labels.length > 1,
            labels: { boxWidth: 12, font: { size: 10 } },
          },
          tooltip: { enabled: false },
        },
      },
    });

    addDataPoint(props.value, props.chartLimit);
    setReady(true);
  });

  onCleanup(() => {
    chart?.destroy();
  });

  createEffect(() => {
    const value = props.value;
    const limit = props.chartLimit;
    if (!ready()) return;
    addDataPoint(value, limit);
  });

  return (
    <div class="flex flex-col gap-1">
      <div class="flex items-center justify-between">
        <span class="text-xs font-medium text-muted-foreground">
          {props.name}
        </span>
        <span class="text-xs font-mono">{displayValue()}</span>
      </div>
      <div class="h-[180px]">
        <canvas ref={canvasRef} />
      </div>
    </div>
  );
}
