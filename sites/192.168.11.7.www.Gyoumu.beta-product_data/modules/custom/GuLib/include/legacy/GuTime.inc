<?php 

class GuTimeHelper
{
  static public function adaptDate($strDate){
    preg_match_all('/(.{4})(.{2})(.{2})/', $strDate, $m);
    $year = $m[1][0];
    $month = $m[2][0];
    $day = $m[3][0];
    
    $month = $month < 10 ? '0' . (int)$month : $month;
    $day = $day < 10 ? '0' . (int)$day : $day;
    
    return $year . '-' . $month . '-' . $day;
  }
}

?>