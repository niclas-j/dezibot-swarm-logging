import { Router, Route } from "@solidjs/router";
import { QueryClient, QueryClientProvider } from "@tanstack/solid-query";
import { Layout } from "@/components/layout";
import HomePage from "@/pages/home";
import SwarmPage from "@/pages/swarm";
import LiveDataPage from "@/pages/live-data";
import SettingsPage from "@/pages/settings";
import LoggingPage from "@/pages/logging";
import NotFoundPage from "@/pages/not-found";

const queryClient = new QueryClient({
  defaultOptions: {
    queries: {
      retry: 1,
      staleTime: 500,
    },
  },
});

export function App() {
  return (
    <QueryClientProvider client={queryClient}>
      <Router root={Layout}>
        <Route path="/" component={HomePage} />
        <Route path="/swarm" component={SwarmPage} />
        <Route path="/livedata" component={LiveDataPage} />
        <Route path="/settings" component={SettingsPage} />
        <Route path="/logging" component={LoggingPage} />
        <Route path="*" component={NotFoundPage} />
      </Router>
    </QueryClientProvider>
  );
}
