<?php
include "header.php";
?>

<div class="loadiframe">
	<button class="loadiframe" id="humidity" data-src="humidity.html" disabled="true">Humidity</button>
	<button class="loadiframe" id="temperature" data-src="temperature.html">Temperature</button>
	<button class="loadiframe" id="results" data-src="results.html">Results</button>
	<button class="loadiframe" id="readings" data-src="readings.html">Readings</button>
</div>

<iframe id='frame' src="humidity.html" width="100%">

<?php
include "footer.php";
?>
