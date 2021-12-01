
<?php
    $servername = "localhost";
    $dbname = "id15814343_airquality";
    $username = "id15814343_jkt";
    $password = "uJ@[x<D)e9S!|T{G";
    function getAllReadings($limit , $CloseL) 
    {
        global $servername, $username, $password, $dbname;
        $conn = new mysqli($servername, $username, $password, $dbname);
        // Check connection
        if ($conn->connect_error) 
        {
          die("Connection failed: " . $conn->connect_error);
        }
        $sql = "SELECT id, Location , CO , NO , Timestamp  FROM sensor_data WHERE Location= ". $CloseL ." order by Timestamp desc limit " . $limit;
        if ($result = $conn->query($sql)) 
        {
          $row = $result;
          return $result;
        }
        else 
        {
          return false;
        }
        $conn->close();
    }
    function getlocation($Close_Location ) 
    {
        global $servername, $username, $password, $dbname;
        $conn1 = new mysqli($servername, $username, $password, $dbname);
        $sql1 = "SELECT Location FROM location_data WHERE nodeid=". $Close_Location;
        if ($result1 = $conn1->query($sql1)) 
        {
            while ($row = $result1->fetch_assoc()) 
            {
                $Close_Location1 = $row["Location"];
                return $Close_Location1;
            }
        }
        else 
        {
          return false;
        }
        $conn->close();
    }
    if ($_GET["readingsCount"])
    {
      $data = $_GET["readingsCount"];
      $data = trim($data);
      $data = stripslashes($data);
      $data = htmlspecialchars($data);
      $readings_count = $_GET["readingsCount"];
    }
    else 
    {
      $readings_count = 20;
    }
    if ($_GET["CloseLocation"])
    {
      $data = $_GET["CloseLocation"];
      $data = trim($data);
      $data = stripslashes($data);
      $data = htmlspecialchars($data);
      $Close_Location = $_GET["CloseLocation"];
      $Close_Location1 =getlocation($Close_Location);
    // $Close_Location1 ="attavar";
    }
    else 
    {
      $Close_Location =2;
      $Close_Location1 ="attavar";
    }
?>

<!DOCTYPE html>
<html>
    <head><meta http-equiv="Content-Type" content="text/html; charset=utf-8">
        <link rel="stylesheet" type="text/css" href="espStyle.css">
        
        
        <script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>
        <script type="text/javascript">
          google.charts.load('current', {'packages':['corechart']});
          google.charts.setOnLoadCallback(drawChart);
    
          function drawChart() {
            var data = google.visualization.arrayToDataTable([
              ['Timestamp', 'CO', 'Light',],

              <?php
              $result = getAllReadings($readings_count,$Close_Location);
                if ($result) {
                while ($row = $result->fetch_assoc()) 
                {
                    echo "['".$row["Timestamp"]."', ".$row["CO"]." , ".$row["NO"]."],";  
                } } 
                ?>
            ]);
            
            var options = {
              title: 'AIR Pollution',
              curveType: 'function',
              legend: { position: 'bottom' },
              hAxis: { 
                        direction: -1,
                        slantedText: true, 
                        slantedTextAngle: 20
                    }
            };
    
            var chart = new google.visualization.LineChart(document.getElementById('curve_chart'));
    
            chart.draw(data, options);
          }
        </script>

        
    </head>
    <header class="header">
        <h1> Air Monitoring System</h1>
        <form method="get">
            <input type="number" name="readingsCount" min="1" placeholder="Number of readings (<?php echo $readings_count; ?>)">
            <input type="number" name="CloseLocation" min="0" placeholder="Close Location(<?php echo $Close_Location1; ?>)">
            <input type="submit" value="UPDATE">
        </form>

    </header>
<body>
    
<div id="curve_chart" style="width: 1600px; height: 500px"></div>

<?php
    echo   '<h2> View Latest ' . $readings_count . ' Readings of ' . $Close_Location1 .'</h2>
            <table  cellspacing="5" cellpadding="5" id="tableReadings">
            <tr>
                    <th rowspan=2 >ID</th>
                    <th rowspan=2 >Location</th>
                    <th colspan="2">Voltage</th>
                    <th rowspan=2 >Timestamp</th>
                </tr>
                <tr>
                    <th>CO</th>
                    <th>NO</th>
                </tr>';
    $result = getAllReadings($readings_count,$Close_Location);
        if ($result) {
        while ($row = $result->fetch_assoc()) 
        {
            $row_id = $row["id"];
            $Location = $row["Location"];
            $CO = $row["CO"];
            $NO = $row["NO"];
            $Timestamp = $row["Timestamp"];
            echo '<tr>
                    <td>' . $row_id . '</td>
                    <td>' . $Location . '</td>
                    <td>' . $CO . '</td>
                    <td>' . $NO . '</td>
                    <td>' . $Timestamp . '</td>
                  </tr>';
        }
        echo '</table>';
        $result->free();
    }
?>
</body>
</html>