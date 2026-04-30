const elStatus = document.getElementById("connection-status");
const elSoil = document.getElementById("soil-moisture");
const elTemp = document.getElementById("temperature");
const elHumidity = document.getElementById("humidity");
const elLastUpdated = document.getElementById("last-updated");
const elThreshold = document.getElementById("threshold");
const elSaveBtn = document.getElementById("btn-save-settings");
const elLogList = document.getElementById("pump-log-list");
const elPumpState = document.getElementById("pump-state");

// 페이지 로드 시 초기 데이터 fetch
async function loadSettings() {
    const res = await fetch("/settings");
    const data = await res.json();
    elThreshold.value = data.threshold;
}

async function loadPumpLogs() {
    const res = await fetch("/pump/logs");
    const logs = await res.json();
    logs.forEach(log => {
        const li = document.createElement("li");
        li.textContent = `${log.timestamp} - ${log.action}`;
        elLogList.appendChild(li);
    });
}

const PUMP_LABEL = {
    [PUMP_IDLE]:    "대기 중",
    [PUMP_PUMPING]: "급수 중",
    [PUMP_SOAKING]: "흡수 대기",
};

const PUMP_CLASS_MAP = {
    [PUMP_IDLE]:    "idle",
    [PUMP_PUMPING]: "pumping",
    [PUMP_SOAKING]: "soaking",
};

// SSE 연결
function connectSSE() {
    const es = new EventSource("/stream");

    es.onopen = () => {
        elStatus.textContent = "SSE 연결됨";
        elStatus.className = "connected";
    };

    es.onmessage = (event) => {
        const data = JSON.parse(event.data);

        if (data.type == TYPE_SENSOR_DATA) {
            elSoil.textContent = data.soil_moisture_pct ?? "--";
            elTemp.textContent = data.air_temperature ?? "--";
            elHumidity.textContent = data.air_humidity ?? "--"
            elLastUpdated.textContent = new Date().toLocaleTimeString("ko-KR");
        } else if (data.type == TYPE_PUMP_STATE) {
            elPumpState.textContent = PUMP_LABEL[data.state] ?? data.state;
            elPumpState.className = PUMP_CLASS_MAP[data.state] ?? "";
        }
    };

    es.onerror = () => {
        elStatus.textContent = "SSE 연결 끊김";
        elStatus.className = "disconnected";
    };
}

/**
 * @param {string} message 
 */
function showToast(message) {
    const toast = document.createElement("div");
    toast.className = "toast";
    toast.textContent = message;
    document.getElementById("toast-container").appendChild(toast);

    // 2s 뒤 fadeOut
    setTimeout(() => {
        toast.classList.add("hide");

        // fadeOut 끝난 뒤 DOM에서 제거
        toast.addEventListener("transitionend", () => toast.remove());
    }, 2000)
}

// 저장 버튼
elSaveBtn.addEventListener("click", async () => {
    const value = parseInt(elThreshold.value, 10);
    await fetch("/settings/threshold", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ threshold: value })
    });
    showToast("저장하였습니다.")
});

loadSettings();
loadPumpLogs();
connectSSE();