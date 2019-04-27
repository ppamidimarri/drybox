<?php

try {
	// Try Connect to the DB with new MySqli object - Params {hostname, userid, password, dbname}
	$mysqli = new mysqli("localhost", "sensor", "sensorpass", "drybox");
	$statement = $mysqli->prepare("select year(stamp), month(stamp), day(stamp), hour(stamp), minute(stamp), second(stamp), temperature_1, temperature_2, temperature_3, temperature_4, temperature_5 from readings");
	$statement->execute(); // Execute the statement.
	$sql_result = $statement->get_result(); // Binds the last executed statement as a result.
	$cols = array(
			array("id" => "1", "label" => "Timestamp", "type" => "datetime"),
			array("id" => "2", "label" => "1", "type" => "number"),
			array("id" => "3", "label" => "2", "type" => "number"),
			array("id" => "4", "label" => "3", "type" => "number"),
			array("id" => "5", "label" => "4", "type" => "number"),
			array("id" => "6", "label" => "5", "type" => "number")
		);
	$rows = array();
	while($row = mysqli_fetch_array($sql_result, MYSQLI_NUM)) {
		$month = $row[1] - 1;
		$rows[] = array("c" => array(
			array("v" => "Date(" . $row[0]. ", " . $month . ", " . $row[2] . ", " . $row[3] .
				", " . $row[4] . ", " . $row[5] . ")", "f" => NULL),
			array("v" => $row[6], "f" => NULL),
			array("v" => $row[7], "f" => NULL),
			array("v" => $row[8], "f" => NULL),
			array("v" => $row[9], "f" => NULL),
			array("v" => $row[10], "f" => NULL)
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
