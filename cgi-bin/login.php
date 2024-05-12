<?php

session_start();

if (isset($_SESSION['username']) && !empty($_SESSION['username'])) {
    header('Location: ./welcome.php');
    exit();
}

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $username = filter_input(INPUT_POST, 'username', FILTER_SANITIZE_STRING);
    $title = filter_input(INPUT_POST, 'title', FILTER_SANITIZE_STRING);

    $_SESSION['username'] = $username;
    $_SESSION['title'] = $title;

    header('Location: welcome.php');
    exit();
}
?>

<!DOCTYPE html>
<html>
<head>
    <title>Login Page</title>
</head>
<body>
    <h1>Login</h1>
    <form action="login.php" method="post">
        <label for="username">Username:</label>
        <input type="text" id="username" name="username" required><br>

        <label for="title">Title:</label>
        <input type="text" id="title" name="title" required><br>

        <input type="submit" value="Login">
    </form>
</body>
</html>
