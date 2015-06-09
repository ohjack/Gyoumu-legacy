<?php
namespace Chte\Processor;

class Yahoo extends Record
{
  public function getRecords()
  {
    foreach ($this->input as $ind => &$row) {
      $pos_count = 0;

      foreach ($row as $hd => &$cell) {
        switch ($hd) {
          case 'POS_lv1':
          case 'POS_lv2':
          case 'POS_lv3':
          case 'POS_lv4':
          case 'POS_lv5':
            $cell = $cell ? $cell : '1';
            break;

          case 'menu_lv1':
          case 'menu_lv2':
          case 'menu_lv3':
          case 'menu_lv4':
          case 'menu_lv5':
            if (!$cell) {
              $cell = $pos_count % 2
                  ? 'OFF' : 'ON';

              $pos_count++;
            }
            break;
        }
      }
    }

    return parent::getRecords();
  }
}