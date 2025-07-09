<?php
$target_dir = "firmware/";
if (!file_exists($target_dir)) {
    mkdir($target_dir, 0755, true);
}

// Validate file upload
if (!isset($_FILES["firmware"]) || $_FILES["firmware"]["error"] !== UPLOAD_ERR_OK) {
    http_response_code(400);
    echo "❌ Upload error.";
    exit;
}

// Check file extension and MIME type
$original_name = basename($_FILES["firmware"]["name"]);
$extension = pathinfo($original_name, PATHINFO_EXTENSION);
$mime = $_FILES["firmware"]["type"];

if (strtolower($extension) !== "bin" || $mime !== "application/octet-stream") {
    http_response_code(400);
    echo "❌ Only .bin files allowed.";
    exit;
}

// Final path using original filename
$target_file = $target_dir . $original_name;

if (move_uploaded_file($_FILES["firmware"]["tmp_name"], $target_file)) {
    echo "✅ Upload successful.\nSaved as: " . $target_file;
} else {
    echo "❌ Upload failed.";
}
?>
