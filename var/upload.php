<!DOCTYPE html>
<html>
<head>
	<title>CGI POST</title>
</head>
<body>
<div style="text-align: center;">
<?php
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Check if a file was uploaded
    if (isset($_FILES['image']) && $_FILES['image']['error'] === UPLOAD_ERR_OK) {
        // Retrieve the file information
        $file_name = $_FILES['image']['name'];
        $file_size = $_FILES['image']['size'];
        $file_tmp = $_FILES['image']['tmp_name'];
        $file_type = $_FILES['image']['type'];

        // Move the uploaded file to a new location
        $target_dir = "uploads/";
        $target_file = $target_dir . basename($file_name);
        move_uploaded_file($file_tmp, $target_file);

        // Display a success message
        echo "<p>The image $file_name has been uploaded.</p>";
    } else {
        // Display an error message
        echo "<p>There was an error uploading the image.</p>";
    }
}
?>
</div>
</body>
</html>

