// GLOBALS
var date = new Date();
var chart;
// END GLOBALS

/**
 * Moves the displayed period to the previous day
 */
function earlier() {
  date.setDate(date.getDate() - 1);
  displayDate(date);
  drawChart(date);
}

/**
 * Moves the displayed period to the next day
 */
function later() {
  date.setDate(date.getDate() + 1);
  displayDate(date);
  drawChart(date);
}

/** 
* Decodes a single data point and returns values as JSON 
* @param array raw data from the backend
* @param offset position in the stream 
*/
function decodeFrame(dataView, offset) {
  const timestamp = dataView.getUint32(offset, true);
  const pm1 = dataView.getUint16(offset + 4, true);
  const pm2_5 = dataView.getUint16(offset + 6, true);
  const pm10 = dataView.getUint16(offset + 8, true);
  return { timestamp: timestamp,               
          pm1: pm1, pm2_5: pm2_5, pm10: pm10 };     
}

/** Decodes a stream of measurements into an array of decoded frames */
function decode(dataView) {
  var offset = 0;
  var result = [];
  var prevFrame;
  while (offset < dataView.byteLength) { 
    const framelen = dataView.getUint8(offset);
    if (framelen != 12) {
      offset += 1;
      continue;
    }        
    const frame = decodeFrame(dataView, offset + 1);
    if (prevFrame && frame.timestamp - prevFrame.timestamp > 15 * 60) {
      result.push({ timestamp: null, pm1: null, pm2_5: null, pm10: null });
    }
    result.push(frame);
    offset += framelen + 1;
    prevFrame = frame;
  }
  return result;
}

/** Updates the date displayed above the chart */
function displayDate(date) {
  const options = {
  weekday: "long",
  year: "numeric",
  month:"long",
  day:"numeric"
  }
  const dateStr = date.toLocaleDateString(window.navigator.language, options);
  document.getElementById("date").innerHTML = dateStr;
}

/** Fetches history of measurements from the server */
async function fetchData(date) {  
  const dd = String(date.getDate()).padStart(2, '0');
  const mm = String(date.getMonth() + 1).padStart(2, '0');
  const yyyy = date.getFullYear();    
  const dateStr = (yyyy + "-" + mm + "-" + dd);    
  const response = await fetch("../log/pm/" + dateStr);
  if (response.status == 200)
    return new DataView(await response.arrayBuffer());
  console.log("Failed to fetch data: " + response.status);
  return new DataView(new ArrayBuffer(0));
} 

function toDataSet(data, valueF) {
  return data.map(d => ({ x: new Date(d.timestamp * 1000.0), y: valueF(d) }));
}

function dataSetPm1(data) {
  return toDataSet(data, v => v.pm1);
}

function dataSetPm2_5(data) {
  return toDataSet(data, v => v.pm2_5);
}

function dataSetPm10(data) {
  return toDataSet(data, v => v.pm10);
}

function dataSetTemperature(data) {
  return toDataSet(data, v => v.temperature);
}

function dataSetHumidity(data) {
  return toDataSet(data, v => v.humidity);
}

function prepareDataSets(measurements) {
  const dataSetOptions = { radius: 0, borderWidth: 1, showLine: true, spanGaps: false };
  return {
    datasets: [
      Object.assign({ 
        label: 'PM 1', 
        borderColor: '#C00000', 
        backgroundColor: 'rgba(255, 70, 50, 1)', 
        data: dataSetPm1(measurements) }, 
        dataSetOptions),
      Object.assign({ 
        label: 'PM 2,5', 
        borderColor: '#FF8C00', 
        backgroundColor: 'rgba(255, 165, 60, 1)', 
        data: dataSetPm2_5(measurements) }, 
        dataSetOptions),
      Object.assign({ 
        label: 'PM 10', 
        borderColor: '#FFB080', 
        backgroundColor: 'rgba(255, 240, 100, 1)', 
        data: dataSetPm10(measurements) }, 
        dataSetOptions),
    ]
  }
}

/** Configures a new Chart object and initializes it with given measurements */
function initChart(measurements, startTime, endTime) {
  chart = Chart.Line('chart', {
    data: prepareDataSets(measurements),
    options: {
      responsive: true,        
      maintainAspectRatio: false,
      legend: {
        labels: {
          fontSize: 13,
        }
      },
      scales: {
        xAxes: [{ 
          type: 'time',             
          time: {          
            unit: 'hour',
            displayFormats: {
              hour: 'k'
            }            
          },
          position: 'bottom',             
          ticks: {          
            fontSize: 13,
            min: startTime,
            max: endTime,
          },
          gridLines: {
            z: 1,
            color: "rgba(70, 70, 70, 0.08)"
          }
        }],      
        yAxes: [{                         
          scaleLabel: {
            display: true,
            labelString: 'μg/m³'
          },
          ticks: {
            fontSize: 13,
            min: 0,              
            maxTicksLimit: 21,   
            suggestedMax: 80,
            beginAtZero: true,
          },
          gridLines: {
            z: 1,
            color: "rgba(70, 70, 70, 0.08)"
          }
        }]
      }
    }
  });
}

/** Loads the new set of measurements into the existing Chart */ 
function updateChart(measurements, startTime, endTime) {
  chart.data = prepareDataSets(measurements);    
  chart.options.scales.xAxes[0].ticks.min = startTime;
  chart.options.scales.xAxes[0].ticks.max = endTime;
  chart.update();
}

/** 
 * Reads measurements from the server and loads it into the chart.
 * If the chart doesn't exist yet, it is initialized.
 */
async function drawChart() {
  var startTime = new Date(date);
  startTime.setHours(0,0,0,0);
  var endTime = new Date(startTime);
  endTime.setDate(startTime.getDate() + 1);        
  const stream = await fetchData(date);
  const measurements = decode(stream);
  
  if (chart == null) 
    initChart(measurements, startTime, endTime);
  else 
    updateChart(measurements, startTime, endTime);
}
  
function load() {
  displayDate(date);
  drawChart(date);
}
  
function warnColor(value, limit1, limit2, ready) {
  if (ready == 0)
    return "#A0A0A0";  // gray
  else if (value <= limit1)
    return "#008400";  // green
  else if (value <= limit2)
    return "#FF8E00";  // orange
  else 
    return "#CF0000";  // red
}

function formatPm(value, limit1, limit2, ready) {
  return "<span style=\"color:" + warnColor(value, limit1, limit2, ready) + "\">" + value + "&nbsp;&nbsp;</span>";
}

function formatValue(value) {
  return (value !== null && !isNaN(value))
    ? value.toLocaleString(undefined, {minimumFractionDigits: 1, maximumFractionDigits: 1})
    : "";
}

function displaySensorValues(temperature, humidity, pm10, pm2_5, pm1, pmReady) {
  document.getElementById("temperature").innerHTML = formatValue(temperature);
  document.getElementById("humidity").innerHTML = formatValue(humidity);        
  document.getElementById("pm10").innerHTML = pm10 !== null ? formatPm(pm10, 40, 100, pmReady) : "";
  document.getElementById("pm2_5").innerHTML = pm2_5 !== null ? formatPm(pm2_5, 25, 80, pmReady) : "";        
  document.getElementById("pm1").innerHTML = pm1 !== null ? formatPm(pm1, 15, 40, pmReady) : "";           
}

function refreshSensorValues() {
  var req = new XMLHttpRequest();  
  req.onreadystatechange = function() {
    if (this.readyState == 4) {
      if (this.status == 200) {
        var data = JSON.parse(this.responseText);
        display(data.temperature, data.humidity, data.pm10, data.pm2_5, data.pm1, data.pmready);
      }
      else {
        display(null, null, null, null, null);
      }       
    }
  }
  req.open('GET', '/sensor');
  req.send();
} 