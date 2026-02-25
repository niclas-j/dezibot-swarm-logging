import { A } from "@solidjs/router";
import { Button } from "@/components/ui/button";

export default function NotFoundPage() {
  return (
    <div class="flex flex-col items-center justify-center gap-4 pt-20">
      <h1 class="text-4xl font-bold">404</h1>
      <p class="text-muted-foreground">Page not found</p>
      <A href="/">
        <Button>Back to Home</Button>
      </A>
    </div>
  );
}
