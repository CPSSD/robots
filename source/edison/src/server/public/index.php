<html>
	<link rel="stylesheet" type="text/css" href="stylesheet.css" />
	<title>Robot Control</title>
	
	<script>
		var angle = 0;
		var magnitude = 0;
		
		function update(){
			angle = document.getElementById("angle").value
			document.getElementById("current-angle").textContent = ("Current Angle: " + angle);
			
			magnitude = document.getElementById("distance").value
			document.getElementById("current-distance").textContent = ("Distance: " + magnitude + " cm/s");
		}
		
		function submit(){
			console.log({"Angle": angle, "Distance": magnitude})
			var request = new XMLHttpRequest();
			request.open("GET", "submit/"+angle+"/"+magnitude, false);
			request.send(null);
		}
		
		function getTime() {
			date = new Date();
			hours = date.getHours();
			if (hours < 10) {
				hours = "0" + hours;
			}
			minutes = date.getMinutes();
			if (minutes < 10) {
				minutes = "0" + minutes;
			}
			seconds = date.getSeconds();
			if (seconds < 10) {
				seconds = "0" + seconds;
			}
			return "[" + hours + ":" + minutes + ":" + seconds + "]";
		}
		
		function updateResponses(){
			console.log("Trying to get JSON from /drive/response/");
			var request = new XMLHttpRequest();
			request.onreadystatechange = function() {
				// If Request Finished and Status is "OK"
				if (request.readyState == 4 && request.status == 200) {
					clearResponseTable();
					jsonResponse = JSON.parse(request.responseText);
					console.log(jsonResponse['Responses']);
					for (var element in jsonResponse['Responses']) {
						parseResponses(jsonResponse['Responses'][element]);
					}
				}
			};
			request.open("GET", "response/", true)
			request.send();
		}
		
		function parseResponses(response){
			addResponse(response['Time'], response['Angle'], response['Magnitude'])
		}
		
		function clearResponseTable(){
			var table = document.getElementById("response-table")
			while (table.lastChild) {
				table.removeChild(table.lastChild);
			}
		}
		
		function addResponse(time, angle, magnitude){		
			// Create and add new row to the table
			var table = document.getElementById("response-table");
			var row = table.insertRow(-1);
			var cell1 = row.insertCell(0);
			cell1.innerHTML = time;
			var cell2 = row.insertCell(1);
			cell2.innerHTML = "Move Response"
			var cell3 = row.insertCell(2);
			cell3.innerHTML = "Angle: " + angle + ""
			var cell4 = row.insertCell(3);
			cell4.innerHTML = "Magnitude: " + magnitude + ""
			
			// Keep scrollbar locked to bottom when adding new response
			document.getElementById('response-div').scrollTop = document.getElementById('response-div').scrollHeight;
		}
		
		function init(){
			console.log("Starting...");
			update();
			updateResponses();
			window.setInterval(updateResponses, 5000)
		}
		
	</script>
	
	<body onload="init()">
		<div class="header"></div>
		
		<div class="title">
			<h1 class="header">CPSSD Robot Project</h1>
		</div>
		
		<div class="content">
			<div class="section">
				<h2 class='title'><u>Controls</u></h1>			
				<p id="current-angle">Current Angle: 0</p>
				<input type="range" id="angle" min="0" max ="359" step="1" value="180" oninput="update()"/>
				
				<p id="current-distance">Distance: 0 cm/s</p>
				<input type="range" id="distance" min="0" max ="200" step="5" value="100" oninput="update()"/>
				
				<br /><br />
				<input class="button" value="Send Command" type="button" id="submit" onclick="submit()" />
			</div>
			<div class="section">
				<h2 class='title'><u>Responses</u></h1>
				<div class="response" id='response-div'>
					<table id='response-table'></table>
				</div>
			</div>
		</div>
	</body>
	
</html>