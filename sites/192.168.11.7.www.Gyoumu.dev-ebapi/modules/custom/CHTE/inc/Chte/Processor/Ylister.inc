<?php
namespace Chte\Processor;

class Ylister extends Record
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
              $cell = 'OFF';
            }
            break;

          case 'title':
            $row['title'] = $row['anchor'] . $row['title'];
            break;

          case 'gallery':
            $path = str_replace("\\", '/', $cell);
            if(preg_match('@^//SHAREPJ/public/Gallery/(.*)@', $path, $m)){
              $cell = '/home/gulei/SmbGallery/' . $m[1];
            }
            break;

          case 'city':
            if($cell == '台灣'){
              $cell = '台湾';
            }
        }
      }
    }

    return parent::getRecords();
  }
}