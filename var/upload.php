<?php

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
	$uploads_dir = './uploads/';
	$filename = basename($_FILES['file']['name']);
	$upload_path = $uploads_dir . $filename;
	if (move_uploaded_file($_FILES['file']['tmp_name'], $upload_path)) {
		http_response_code(200);
	} else {
		http_response_code(500);
	}
}

?>
