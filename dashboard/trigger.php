<?php
if ($_SERVER["REQUEST_METHOD"] === "POST" && isset($_POST["filename"]) && isset($_POST["topic"])) {
    $filename = basename($_POST["filename"]);
    $raw_topic = $_POST["topic"];
    $topic = preg_replace('/[^a-zA-Z0-9_\/-]/', '', $raw_topic); // basic sanitization
    $broker = "172.203.220.96";

    if (strpos($topic, "esp/update/") !== 0) {
        http_response_code(400);
        echo "? Invalid topic. Must start with 'esp/update/'";
        exit;
    }

    $cmd = "mosquitto_pub -h $broker -t \"$topic\" -m \"$filename\"";
    exec($cmd, $output, $status);

    if ($status === 0) {
        echo "? OTA triggered on topic '$topic' with firmware '$filename'";
    } else {
        http_response_code(500);
        echo "? Failed to trigger OTA.\nCommand: $cmd\nOutput:\n" . implode("\n", $output);
    }
} else {
    http_response_code(400);
    echo "? Invalid request. Filename or topic missing.";
}
?>
