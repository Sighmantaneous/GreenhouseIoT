
String homepage = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="utf8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Herbinside</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      background-color: #f4f4f4;
      text-align: center;
      margin: 0;
      padding: 20px;
    
    }

    .dashboard {
      display: flex;
      flex-wrap: wrap;
      justify-content: center;
      gap: 30px;
    }

    .card {
      background: #fff;
      border-radius: 12px;
      //box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
      padding: 15px;
      
    }

    .label {
      font-size: 1.2em;
      font-weight: bold;
      margin-top: 15px;
      color: #333;
    }

    .value {
      font-size: 2em;
      color: #0077aa;
      margin-top: 5px;
    }

    iframe {
      border: 1px solid #cccccc;
      width: 450px;  
      height: 250px; 
    }
  </style>
</head>
<body>
  <h1>🌿 Herbinside IoT Greenhouse</h1>

  <div class="dashboard">

    <!-- Temperature Section -->
    <div class="card">
      <iframe src="https://thingspeak.com/channels/2712952/charts/1?bgcolor=%23ffffff&color=%23008080&days=1&dynamic=true&results=60&title=Temperature&type=spline&xaxis=Time&yaxis=Celsius&yaxismax=35&yaxismin=05"></iframe>>
      <div class="label">Temperature</div>
      <div class="value" id="tempValue">-- °C</div>
    </div>

    <!-- Humidity Section -->
    <div class="card">
      <iframe src="https://thingspeak.com/channels/2712952/charts/2?bgcolor=%23ffffff&color=%23008080&days=1&dynamic=true&results=40&title=Humidity&type=spline&xaxis=Time&yaxis=Humidity+%25&yaxismax=99&yaxismin=20"></iframe>
      <div class="label">Humidity</div>
      <div class="value" id="humidityValue">-- %</div>
    </div>

    <!-- Soil Moisture Section -->
    <div class="card">
      <iframe src="https://thingspeak.com/channels/2712952/charts/3?bgcolor=%23ffffff&color=%23008080&days=1&dynamic=true&results=60&title=Soil+Moisture&type=spline&xaxis=Time&yaxis=Moisture+%25&yaxismax=100&yaxismin=05"></iframe>
      <div class="label">Soil Moisture</div>
      <div class="value" id="moistureValue">-- %</div>
    </div>

  </div>

  <script> 

    function fetchData() { 

      fetch('/getValues') 

        .then(response => response.json()) 

        .then(data => { 

          document.getElementById("tempValue").textContent = data.temperature + " °C"; 

          document.getElementById("humidityValue").textContent = data.humidity + " %"; 

          document.getElementById("moistureValue").textContent = data.moisture + " %"; 

        }) 

        .catch(error => { 

          console.error('Error fetching sensor data:', error); 

        }); 

    } 

  

    setInterval(fetchData, 10000); 

    fetchData(); 

  </script>
</body>
</html>
  )rawliteral";
