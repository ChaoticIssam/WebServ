<?php

if ($_SERVER["REQUEST_METHOD"] === "POST") {
    if (isset($_POST["name"]) && isset($_POST["title"])){
        $name = $_POST["name"];
        $title = $_POST["title"];

        echo "<html><body>";
        echo "<h1>Received Data:</h1>";
        echo "<p>Name: " . htmlspecialchars($name) . "</p>";
        echo "<p>Title: " . htmlspecialchars($title) . "</p>";
        echo "</body></html>";
    }
    else {
        echo "<html><body>";
        echo "<h1>No data received</h1>";
        echo "</body></html>";
    }

} else if ($_SERVER["REQUEST_METHOD"] === "GET") {
    if (isset($_GET["name"]) && isset($_GET["title"])) {
        $name = $_GET["name"];
        $title = $_GET["title"];
    
        echo "<html><body>";
        echo "<h1>Received Data:</h1>";
        echo "<p>Name: " . htmlspecialchars($name) . "</p>";
        echo "<p>Title: " . htmlspecialchars($title) . "</p>";
        echo "</body></html>";
    }
    else {
        echo "<html><body>";
        echo "<h1>No data received</h1>";
        echo "</body></html>";
    }
} else {
    header("HTTP/1.1 405 Method Not Allowed");
    echo "Method not allowed";
}

?>  