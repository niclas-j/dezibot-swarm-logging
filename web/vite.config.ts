import { defineConfig } from "vite";
import solid from "vite-plugin-solid";
import tailwindcss from "@tailwindcss/vite";
import { resolve } from "path";

export default defineConfig({
  plugins: [solid(), tailwindcss()],
  resolve: {
    alias: {
      "@": resolve(__dirname, "./src"),
    },
  },
  build: {
    outDir: "../data",
    emptyOutDir: true,
    target: "es2020",
  },
  server: {
    proxy: {
      "/getSwarmData": "http://192.168.1.1",
      "/getEnabledSensorValues": "http://192.168.1.1",
      "/logging": "http://192.168.1.1",
      "/settings": "http://192.168.1.1",
    },
  },
});
