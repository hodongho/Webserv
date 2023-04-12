<!DOCTYPE html>
<html>
<head>
	<title>PHP CGI</title>
</head>
<body>
<div style="text-align: center;">
<?php
	echo "<p>PHP CGI Test Page</p>";
	if ($_SERVER['REQUEST_METHOD'] == 'POST') {
		$name = $_POST['name'];
		echo "<h1>Hello, $name!</h1>";
	} else {
		echo "<p>Error: Invalid request method.</p>";
	}
	$dateString = date("Y-m-d", time());
	echo "<p>Today: $dateString</p>";
?>
</div>
</body>
</html>
