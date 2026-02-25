import { A, useLocation } from "@solidjs/router";
import type { ParentProps } from "solid-js";
import { Button } from "@/components/ui/button";
import { cx } from "@/lib/cva";

const navItems = [
  { href: "/", label: "Home" },
  { href: "/swarm", label: "Swarm" },
  { href: "/livedata", label: "Live Data" },
  { href: "/settings", label: "Settings" },
  { href: "/logging", label: "Logs" },
];

export function Layout(props: ParentProps) {
  const location = useLocation();

  return (
    <div class="min-h-screen bg-background">
      <header class="sticky top-0 z-50 border-b bg-primary text-primary-foreground">
        <div class="flex h-12 items-center gap-2 px-4">
          <span class="font-semibold text-sm mr-2">Dezibot</span>
          <nav class="flex gap-1">
            {navItems.map((item) => (
              <A href={item.href}>
                <Button
                  variant={
                    location.pathname === item.href ? "secondary" : "ghost"
                  }
                  size="sm"
                  class={cx(
                    location.pathname !== item.href &&
                      "text-primary-foreground hover:text-primary-foreground hover:bg-primary-foreground/20",
                  )}
                >
                  {item.label}
                </Button>
              </A>
            ))}
          </nav>
        </div>
      </header>
      <main class="p-4">{props.children}</main>
    </div>
  );
}
