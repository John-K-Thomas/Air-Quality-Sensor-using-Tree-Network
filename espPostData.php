<?php
    $servername = "localhost";
    $dbname = "id15814343_airquality";
    $username = "id15814343_jkt";
    $password = "uJ@[x<D)e9S!|T{G";
    $api_key_value = "tPmAT5Ab3j7F9";
    $api_key= $Location = $CO =$NO = "";

  if ($_SERVER["REQUEST_METHOD"] == "GET") {
    $api_key = test_input($_GET["api_key"]);
    if($api_key == $api_key_value) {
      $Location = test_input($_GET["Location"]);
      $CO = test_input($_GET["CO"]);
      $NO = test_input($_GET["NO"]);
      $result = insertReading($Location,$CO,$NO);
      echo $result;
    }
    else {
      echo "Wrong API Key provided.";
    }
  }
  else {
    echo "No data posted with HTTP POST.";
  }

  function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
  }
   function insertReading($Location,$CO,$NO) {
    global $servername, $username, $password, $dbname;

    // Create connection
    $conn = new mysqli($servername, $username, $password, $dbname);
    // Check connection
    if ($conn->connect_error) {
      die("Connection failed: " . $conn->connect_error);
    }

    $sql = "INSERT INTO sensor_data(Location,CO,NO) VALUES ('" . $Location . "','" . $CO . "','" . $NO . "')";

    if ($conn->query($sql) === TRUE) {
      return "New record created successfully";
    }
    else {
      return "Error: " . $sql . "<br>" . $conn->error;
    }
    $conn->close();
  }
?>
