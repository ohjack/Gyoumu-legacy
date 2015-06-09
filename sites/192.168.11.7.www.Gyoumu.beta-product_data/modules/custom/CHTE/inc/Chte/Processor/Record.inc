<?php
namespace Chte\Processor;

class Record extends _Abstract
{
  public function getRecords()
  {
    $recs = array();

    foreach ($this->input as $each_input) {
      $src = array();
      foreach ($each_input as $key => $val) {
        $val = trim($val);

        if (strlen($val)) {
          $src[$key] = $val;
        }
      }

      $src = array_merge($this->defaults, $src);
      $rec = array();

      foreach ($this->headers as $hd) {
        if (!key_exists($hd, $rec)) {
          $rec[$hd] = '';
        }
      }

      foreach ($src as $key => $val) {
        $rec[$key] = $val;
        $rec[$key . '_LOWERCASE'] = strtolower($val);
        $rec[$key . '_UPPERCASE'] = strtoupper($val);
      }

      foreach ($rec as $key => $val) {
        $lines = explode("\n", $val);

        foreach($lines as $line_key => &$line){
          $line = trim($line);
          if(!strlen($line)){
            unset($lines[$line_key]);
          }
        }
        $rec[$key . '_LINES'] = array_values($lines);
      }
      $recs[] = $rec;
    }
    return $recs;
  }
}