// To create literal strings stored in flash memory enclose your HTML code between 
// F(R"=====( HTML code here )=====");
// If you have 1 reading then you probably have 2 literal strings
// If you have 2 readings then you probably have 3 literal strings etc.

<!DOCTYPE html>
<html lang="en" > <!-- Needed for Screenreaders !-->
<head>
<!-- UTF-8 character set covers most characters in the world -->
 <meta charset="utf-8">
 <!-- Make page respond to screen size !-->
 <meta name="viewport" content="width=device-width, initial-scale=1, viewport-fit=cover">
<!--Include a Title. Used by Search Engines -->
<title> Temperature Reading WebServer </title>
<style>
 
 <!--choose good contrast between background and foreground colours
 body { 
background-color: DodgerBlue
 }
.flex-Container{
 display: flex;
 flex-direction: column;
 background-color: DodgerBlue;
 align-items: center;
 }
 h1{
 font: bold;
 font-size: 40px;
 font-family: Arial;
 color: navy;
text-align: center;
 }
 p{
 font-size: 25px;
 font-family: Arial;
 color: navy;
text-align: center;
 } 
 th, td {
 font-size: 25px;
 padding: 8px;
 text-align: left;

 border-bottom: 1px solid #ddd;
}
</style>
</head>
<body>
<div class="flex-Container">
<h1> Temperature Example Website </h1>
 
<p>Welcome to my website displaying the temperature in ATU</p>
 
<table>
 <tr>
 <th>Sensor</th>
 <th>Value</th>
 <th>Unit</th>
 </tr>
 <tr>
 <td>Temperature</td>
 <td> 23.4</td> 

<<<<<<< HEAD
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
=======
String homePagePart2 = F(R"=====( <td>Degree Cel</td>
 </tr>
 </table>
 </div>
>>>>>>> 3cab1d0b90d9e4760a65242ad08bb4bf88628455
</body>
</html>
)=====");


