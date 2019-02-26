<?php

try {
	// Try Connect to the DB with new MySqli object - Params {hostname, userid, password, dbname}
	$mysqli = new mysqli("localhost", "sensor", "sensorpass", "drybox");
	$statement = $mysqli->prepare("select * from readings");
	$statement->execute(); // Execute the statement.
	$sql_result = $statement->get_result(); // Binds the last executed statement as a result.
	$cols = array(
			array("id" => "1", "label" => "ID", "type" => "number"),
			array("id" => "2", "label" => "Date", "type" => "date"),
			array("id" => "3", "label" => "Time", "type" => "timeofday"),
			array("id" => "4", "label" => "Sensor", "type" => "number"),
			array("id" => "5", "label" => "Humidity", "type" => "number"),
			array("id" => "6", "label" => "Temperature", "type" => "number")
		);
	$rows = array();
	while($row = mysqli_fetch_array($sql_result, MYSQLI_NUM)) {
		$date_pieces = explode("-", $row[1]);
		$month = $date_pieces[1] - 1;
		$time_pieces = explode(":", $row[2]);
		$rows[] = array("c" => array(
			array("v" => $row[0], "f" => NULL),
			array("v" => "Date(" . $date_pieces[0]. ", " . $month . ", " . $date_pieces[2] . ")", "f" => NULL),
			array("v" => $time_pieces, "f" => NULL),
			array("v" => $row[3], "f" => NULL),
			array("v" => $row[4]), "f" => NULL,
			array("v" => $row[5], "f" => NULL)
		));
	}
	mysqli_free_result($sql_result);
	echo '{"cols": ' . json_encode($cols) . ', "rows": ' . json_encode($rows) . '}';
} catch (mysqli_sql_exception $e) { // Failed to connect? Lets see the exception details..
	echo "MySQLi Error Code: " . $e->getCode() . "<br />";
	echo "Exception Msg: " . $e->getMessage();
	exit(); // exit and close connection.
}

$mysqli->close(); // finally, close the connectionCopy

?>
