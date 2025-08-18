<html>
<head>
	<title>Teste do site do WebSocket</title>
	<meta charset="UTF-8">
	<link rel="stylesheet" href="style.css">
</head>
<body>
	<h4>Temperatura: <span id="temperature">0 °C</span></h4>
	<h4>Umidade: <span id="humidity">0%</span></h4>

	<button id="ledButton" onclick="toggleLed()">LED OFF</button>
	<br><br>

	<!-- Botões dos Relés -->
	<div id="reles">
		<h4>Relés</h4>
		<script>
			for (let i = 1; i <= 8; i++) {
				document.write(`<button id="rele${i}" onclick="toggleRele(${i})">Relé ${i} OFF</button><br><br>`);
			}
		</script>
	</div>

	<script>
		var socket = new WebSocket('ws://127.0.0.1:81');

		socket.onmessage = function(event) {
			console.log('Mensagem recebida: ' + event.data);
			const data = event.data.split(':');
			const msg = data[0] || "";
			const sensor = data[1] || "";

			if(sensor == "led"){
				var button = document.getElementById("ledButton");
				button.innerHTML = msg == "1" ? "LED ON" : "LED OFF";
			}
			else if(sensor == "dht"){
				var parts = msg.split(",");
				document.getElementById("temperature").innerHTML = parts[0] + " °C";
				document.getElementById("humidity").innerHTML = parts[1] + "%";
			}
			else if(sensor.startsWith("rele")) {
				let index = sensor.replace("rele", "");
				let button = document.getElementById("rele" + index);
				button.innerHTML = `Relé ${index} ${msg == "1" ? "ON" : "OFF"}`;
			}
		}; 

		function toggleLed() {
			var button = document.getElementById("ledButton");
			var status = button.innerHTML.includes("OFF") ? "1" : "0";
			socket.send(status + ":led:esp:localhost");
		}

		function toggleRele(id) {
			var button = document.getElementById("rele" + id);
			var status = button.innerHTML.includes("OFF") ? "1" : "0";
			socket.send(status + `:rele${id}:esp:localhost`);
		}
	</script>
</body>
</html>
<?php
	echo "Olá povos do universo, contemplem o mago dos computing @gabc.lima!";
?>
