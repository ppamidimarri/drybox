<html> <head> <title>Drybox Status</title>
<link rel="stylesheet" type="text/css" href="css/drybox_theme.css">
<script>
	function iframeChange() {
		var buttons = document.querySelector(".loadiframe"),
		iframe = document.getElementById('frame');

		buttons.addEventListener("click", function (e) {
			iframe.src = e.target.dataset.src;

			document.querySelectorAll('.loadiframe button').forEach(btn => {
				if (e.target == btn) {
					btn.disabled = true;
				} else {
					btn.disabled = false;
				}
			});
		});
	}
	window.onload = iframeChange;
</script>
</head> <body>
