function formatUptime(ms) {
    const s = Math.floor(ms / 1000);
    const h = Math.floor(s / 3600);
    const m = Math.floor((s % 3600) / 60);
    const sec = s % 60;
    if (h > 0) return h + "h " + m + "m " + sec + "s";
    if (m > 0) return m + "m " + sec + "s";
    return sec + "s";
}

function formatLastSeen(ms) {
    if (ms < 1000) return "just now";
    if (ms < 60000) return Math.floor(ms / 1000) + "s ago";
    return Math.floor(ms / 60000) + "m ago";
}

async function fetchSwarmData() {
    try {
        const response = await fetch("/getSwarmData");
        if (!response.ok) throw new Error("Failed to fetch");
        const devices = await response.json();

        document.getElementById("device-count").textContent =
            "Devices: " + devices.length +
            " (" + devices.filter(d => d.online).length + " online)";

        const tbody = document.getElementById("swarm-body");
        tbody.innerHTML = "";

        devices.sort((a, b) => a.mac.localeCompare(b.mac));

        devices.forEach(device => {
            const tr = document.createElement("tr");
            tr.className = device.online ? "online" : "offline";
            if (device.online) {
                tr.style.cursor = "pointer";
                tr.onclick = function() {
                    window.location.href = "/livedata?mac=" + encodeURIComponent(device.mac);
                };
            }
            tr.innerHTML =
                "<td><span class='status-dot " + (device.online ? "dot-online" : "dot-offline") + "'></span>" + (device.online ? "Online" : "Offline") + "</td>" +
                "<td class='mac'>" + device.mac + "</td>" +
                "<td>" + device.counter + "</td>" +
                "<td>" + formatUptime(device.uptime) + "</td>" +
                "<td>" + formatLastSeen(device.lastSeen) + "</td>";
            tbody.appendChild(tr);
        });
    } catch (e) {
        console.error("Error fetching swarm data:", e);
    }
}

setInterval(fetchSwarmData, 1000);
fetchSwarmData();
