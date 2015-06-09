<?php

class CsvController
{
  private $hds;
  public function __construct($hds)
  {
    $this->hds = $hds;
  }

  public function prepareRecs($recs)
  {
    $newRecs = array();
    foreach($recs as &$rec){
      $newRec = array();

      foreach($this->hds as $hd){
        if(isset($rec[$hd])){
          $newRec[$hd] = $rec[$hd];
        }
        else{
          $newRec[$hd] = null;
        }
      }
      $newRecs[] = $newRec;
    }
    return $newRecs;
  }

  public function convArray2Csv($recs)
  {
    $recs = $this->prepareRecs($recs);

    $csv = array();
    $csv[] = $this->hds;
    $csv = array_merge($csv, $recs);

    foreach($csv as &$row){
      foreach($row as &$cell){
        if(preg_match('/[",\n]/', $cell)){
          $cell = '"' . str_replace('"', '""', $cell) . '"';
        }
      }
      $row = implode($row, ",");
    }

    return implode($csv, "\n");
  }
}