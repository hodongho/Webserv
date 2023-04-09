<!DOCTYPE html>
<html>
<head>
	<title>CGI POST</title>
</head>
<body>
<div style="text-align: center;">
<?php
	if ($_SERVER['REQUEST_METHOD'] == 'POST') {
		$name = $_POST['name'];
		echo "<h1>Hello, $name!</h1>";
	} else {
		echo "<p>Error: Invalid request method.</p>";
	}
?>
</div>
</body>
</html>
