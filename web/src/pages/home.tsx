import { A } from "@solidjs/router";
import { Button } from "@/components/ui/button";
import {
  Card,
  CardHeader,
  CardTitle,
  CardDescription,
} from "@/components/ui/card";

const pages = [
  {
    href: "/swarm",
    title: "Swarm Devices",
    description: "View connected ESP-NOW devices and their status",
  },
  {
    href: "/livedata",
    title: "Live Data",
    description: "Real-time sensor data visualization with charts",
  },
  {
    href: "/settings",
    title: "Settings",
    description: "Enable or disable sensor functions for live data",
  },
  {
    href: "/logging",
    title: "Logs",
    description: "Browse and filter system log entries",
  },
];

export default function HomePage() {
  return (
    <div class="max-w-2xl mx-auto space-y-6">
      <div>
        <h1 class="text-2xl font-bold">Dezibot Debug Server</h1>
        <p class="text-muted-foreground mt-1">
          ESP-NOW swarm monitoring and sensor debugging
        </p>
      </div>

      <div class="grid gap-4 sm:grid-cols-2">
        {pages.map((page) => (
          <A href={page.href} class="no-underline">
            <Card class="h-full transition-colors hover:bg-muted/50 cursor-pointer">
              <CardHeader>
                <CardTitle>{page.title}</CardTitle>
                <CardDescription>{page.description}</CardDescription>
              </CardHeader>
            </Card>
          </A>
        ))}
      </div>
    </div>
  );
}
