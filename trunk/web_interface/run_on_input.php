<?
if (get_magic_quotes_gpc()) {
    $process = array(&$_GET, &$_POST, &$_COOKIE, &$_REQUEST);
    while (list($key, $val) = each($process)) {
        foreach ($val as $k => $v) {
            unset($process[$key][$k]);
            if (is_array($v)) {
                $process[$key][stripslashes($k)] = $v;
                $process[] = &$process[$key][stripslashes($k)];
            } else {
                $process[$key][stripslashes($k)] = stripslashes($v);
            }
        }
    }
    unset($process);
}

echo "<H1> Run compiler on input </h1>";

$f = file_get_contents("syntax.txt");
echo "Syntax is : <pre>";
echo htmlspecialchars($f);
echo "</pre>";

$input='';
if (isset($_POST['input'])) $input=$_POST['input'];
$input = str_replace("\r", "", $input);

echo "<form method=post>";
echo "<textarea rows=3 cols=20 name='input'>$input</textarea>";
echo "<input type=submit>";
echo "</form>";

 echo "<h2> graph (in case of error may be incomplete) will be <a href='graph.png'> here </a> but wait for results </h2>";
if ($input != "") {
 echo "Running on input :<pre>\n" .htmlspecialchars($input)."</pre><br>\n";
 flush();


 $f = fopen("input", "w");
 fwrite($f, $input);
 fclose($f);
 echo "<h2> Run: </h2>\n";
 echo "<pre style='font-size: 7px;'>";
 system("make run 2>&1", $code);
 echo "</pre>";

 echo "Done<br/>";
 echo "Exitcode was $code";
 if ($code==0) {
    echo "<h2> <font color=green> ACCEPTED </font> </h2>";
  } else {
    echo "<h2> <font color=red> COMPILE ERROR (see output) </font> </h2>";
  }

 echo "<h2> graph (in case of error may be incomplete) is <a href='graph.png'> here </a> </h2>";

 echo "<h2>Output was: </h2><pre style='font-size: 8px'>";
 $data = file_get_contents("output");
 echo htmlspecialchars($data);
 echo "</pre>";
 echo "graph (if any) is <a href='graph.png'> here </a>";
}


