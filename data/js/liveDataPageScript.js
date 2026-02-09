const params = new URLSearchParams(window.location.search);
const remoteMac = params.get("mac");
const fetchUrl = remoteMac
    ? "/getEnabledSensorValues?mac=" + encodeURIComponent(remoteMac)
    : "/getEnabledSensorValues";

if (remoteMac) {
    const banner = document.getElementById("remote-banner");
    if (banner) {
        banner.textContent = "Remote device: " + remoteMac;
        banner.style.display = "block";
    }
    const title = document.querySelector("h1");
    if (title) title.textContent = "Live Data - " + remoteMac;
}

// Slider handling to adjust chartLimit
const sliderInput = document.getElementById('slider-input');
const numberInput = document.getElementById('slider-value');

function updateChartLimit(value) {
    chartLimit = parseInt(value, 10);
    if (chartLimit < 50 && chartLimit > 2000) {
        return;
    }
    numberInput.value = chartLimit;
    sliderInput.value = chartLimit;
    console.log(`Chart limit updated to: ${chartLimit}`);
}

sliderInput.addEventListener('input', (event) => updateChartLimit(event.target.value));
numberInput.addEventListener('input', (event) => updateChartLimit(event.target.value));

// Chart management
const charts = {};
const dataPoints = {};
let chartLimit = 100;
let xVal = 0;

// Data fetching and DOM update
async function fetchSensorData() {
    try {
        const response = await fetch(fetchUrl);
        if (!response.ok) throw new Error('Failed to fetch data');
        const sensors = await response.json();
        const container = document.getElementById('sensor-container');

        sensors.forEach((sensor, index) => {
            const sensorId = `chartContainer-${index}`;
            const valueElementId = `sensor-value-${sensorId}`;

            if (!document.getElementById(sensorId)) {
                const sensorDiv = document.createElement('div');
                sensorDiv.className = 'sensor';
                sensorDiv.innerHTML = `
                    <div>${sensor.name}</div>
                    <div class="sensor-value" id="${valueElementId}">${JSON.stringify(sensor.value)}</div>
                    <div id="${sensorId}" style="height: 250px; width:90%;"></div>
                `;
                container.appendChild(sensorDiv);

                if (!charts[sensorId]) {
                    createChart(sensorId, sensor.name);
                }
            } else {
                const valueElement = document.getElementById(valueElementId);
                if (valueElement) {
                    if (sensor.name.includes('TiltDirection')) {
                        valueElement.textContent = JSON.stringify(tiltDirectionToString(sensor.value));
                    } else {
                        valueElement.textContent = JSON.stringify(sensor.value);
                    }
                }
            }

            handleIncomingData(sensorId, sensor.value);
        });

        xVal++;
    } catch (error) {
        console.error('Error fetching sensor data:', error);
    }
}

// maps numerical direction values to strings
function tiltDirectionToString(value) {
    switch (value) {
        case '0':
            return 'Front';
        case '1':
            return 'Left';
        case '2':
            return 'Right';
        case '3':
            return 'Back';
        case '4':
            return 'Neutral';
        case '5':
            return 'Flipped';
        case '6':
            return 'Error';
        default:
            return "Unknown";
    }
}

// Chart creation
function createChart(chartId, sensorName) {
    dataPoints[chartId] = [[], [], []];
    charts[chartId] = new CanvasJS.Chart(chartId, {
        axisY: { title: "Value" },
        data: [
            createDataSeries("x", dataPoints[chartId][0]),
            createDataSeries("y", dataPoints[chartId][1]),
            createDataSeries("z", dataPoints[chartId][2])
        ],
        backgroundColor: "transparent"
    });
}

// Data series factory function
function createDataSeries(name, points) {
    return {
        type: "line",
        dataPoints: points,
        showInLegend: true,
        name: name
    };
}

// Data processing and chart update
function handleIncomingData(chartId, value) {
    if (!charts[chartId]) return;

    const numericValue = parseFloat(value);

    if (!isNaN(numericValue)) {
        dataPoints[chartId][0].push({ x: xVal, y: numericValue });
    } else if (typeof value === 'string' && value.includes(',')) {
        const parts = value.split(',')
            .map(part => parseFloat(part.split(':')[1]))
            .filter(num => !isNaN(num));

        parts.forEach((part, index) => {
            if (index < 3 && dataPoints[chartId][index]) {
                dataPoints[chartId][index].push({ x: xVal, y: part });
            }
        });
    } else {
        console.warn('Unknown data format:', value);
        return;
    }

    // Maintain data point limits
    dataPoints[chartId].forEach(dataSeries => {
        while (dataSeries.length > chartLimit) {
            dataSeries.shift();
        }
    });

    charts[chartId].render();
}

// Start periodic updates
setInterval(fetchSensorData, remoteMac ? 1000 : 100);
fetchSensorData();
