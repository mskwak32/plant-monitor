const _MAX_POINTS = 20;

/**
 * 차트 생성
 * @param {HTMLCanvasElement} canvas 
 * @param {string} label 
 * @param {string} color 
 * @returns Chart
 */
function createChart(canvas, label, color) {
    return new Chart(canvas, {
        type: "line",
        data: {
            labels: [],
            datasets: [{
                label: label,
                data: [],
                borderColor: color,
                backgroundColor: color + "22",      // 선 아래 영역 색(22:투명도)
                tension: 0.3,
                pointRadius: 3,
            }]
        },
        options: {
            animation: false,
            responsive: true,
            maintainAspectRatio: false,     // chart-wrap의 height를 그대로 따름
            scales: {
                y: { beginAtZero: false }
            }
        }
    });
}

/**
 * 차트 데이터 추가
 * @param {Chart} chart 
 * @param {string} label 
 * @param {number} value 
 */
function addDataPoint(chart, label, value) {
    chart.data.labels.push(label);
    chart.data.datasets[0].data.push(value);

    if (chart.data.labels.length > _MAX_POINTS) {
        chart.data.labels.shift();
        chart.data.datasets[0].data.shift();
    }

    chart.update();
}