<?php
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
if (!isset($_POST['syntax'])) {
  echo "<h1> Please enter your grammar </h1><br/>";
  echo "Example <pre> ";
  echo htmlspecialchars(
  "input format is 0:<LHSnon_terminal> (<RHS_non_terminal>|\"terminal_string\")*

0:<S><N><E>\"$\\n\"
0:<E><E>\"+\"<T>
0:<E><T>
0:<T>\"A\"
0:<T>\"(\"<E>\")\"
0:<N><W>
0:<W>\" \"
0:<W>");
  echo "</pre>";
  echo "<form method=post action='index.php'>
  <textarea name='syntax' rows=10 cols=40></textarea> <br/>
  <input type=submit>
  </form>";
} else {
  $data = $_POST['syntax'];
  $data = str_replace("\r", "", $data);
  $hash = "data/".md5($data);

  //system("rm -rf data/*");

  echo "making directory ...\n";
  mkdir($hash);
  echo "done<br>"; flush();

  echo "writing syntax data ...\n";
  $f=fopen("$hash/syntax.txt", "w");
  fwrite($f, $data);
  fclose($f);
  echo "done<br>\n"; flush();

  echo "Copying files...\n";
  system("cp Makefile run_on_input.php $hash\n");
  echo "done<br>\n"; flush();

  echo "Making ... <pre>\n";
  system("cd $hash && make 2>&1", $retval);
  echo "</pre> retval $retval; done<br>\n"; flush();
  if ($retval==0) {
    echo "<h2> <font color=green> Compiler for grammar sucesfully built </font> </h2>";
  } else {
    echo "<h2> <font color=ref> There were errors - see make log </font> </h2>";
  }

  echo "<h2> Grammar is available <a href=\"$hash/\"> here </a> </h2>\n";

}
