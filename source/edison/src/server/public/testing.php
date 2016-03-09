<html>
	<link rel="stylesheet" type="text/css" href="stylesheet.css" />
	<title>Robot Control</title>
	
	<script>
		addEventListener("click", getClickPosition, false);
		var centerX, centerY, sphere, rect;
		var angle = 0;
		var distance = 0;
		
		function getClickPosition(e){
			sphere = document.getElementById("sphere");
			rect = sphere.getBoundingClientRect();
			centerX = rect.left + ((rect.right-rect.left)/2)
			centerY = rect.top + ((rect.bottom-rect.top)/2)
			
			console.log(rect.bottom-rect.top);
			if (distance(centerX, centerY, e.pageX, e.pageY) < (rect.bottom-rect.top)/2){
				printAngle(e.pageX, e.pageY);
			}
		}
		
		function distance(x1, y1, x2, y2){
			return Math.sqrt(Math.pow(x2 - x1, 2) + Math.pow(y2 - y1, 2));
		}
		
		function printAngle(x, y){
			
			angle = Math.floor(getAngle(x, y, centerX, centerY));
			
			document.getElementById("angle-text").textContent = ("Current Angle: " + angle);
		}
		
		function toDegrees(angle){
			return angle * (180.0 / Math.PI);
		}
		
		function toRadians(angle){
			return angle * (Math.PI / 180);
		}
		
		function getAngle(x1, y1, x2, y2){
			var h = distance(x1, y1, x2, y2);
			var dist = distance(centerX, centerY-h, x1, y1)/2;
			var angle = Math.asin(toRadians((dist * toDegrees(Math.sin(toRadians(90)))) / h)) * 2;
			angle = toDegrees(angle);
			if (x1 < x2){
				angle = 360-angle;
			}
			
			return angle;
		}
		
		function update(){
			distance = document.getElementById("distance").value
			document.getElementById("current-distance").textContent = ("Distance: " + distance + " cm/s");
		}
		
		function submit(){
			console.log({"Angle": angle, "Distance": distance})
			var request = new XMLHttpRequest();
			request.open("GET", "control/?angle="+angle+"&distance="+distance, false);
			request.send(null);
		}
	</script>
	
	<body>
		<div class="centered">
			
			<div class="sphere" id="sphere"></div>
			<p id="angle-text">Current Angle: 0</p>
			<p id="current-distance">Distance: 0 cm/s</p>
			<input type="range" id="distance" min="0" max ="100" step="5" value="0" onchange="submit()" oninput="update()"/>
		</div>
	</body>
	
</html>