<?php
namespace Oerp\JS;

class Helper
{
  static public function flatJsSend($nested, $id = null)
  {
    if (!is_array($nested) && !is_object($nested)) {
      return null;
    }

    $nested = (array)$nested;
    if(count($nested) == 1 && isset($nested['#value'])){
      $nested = (array)$nested['#value'];
    }

    //render records
    if ($id === (string)(int)$id || strpos($id, '0-') === 0) {
      //  records to delete
      if (isset($nested['#delete'])) {
        return ($id == 0)
            ? null
            : array(2, (int)$id, false);
      }

      return ($id == 0)
          ? array(0, 0, self::flatJsSend($nested['#value']))
          : array(1, (int)$id, self::flatJsSend($nested['#value']));
    }

    //render single-value fields
    if (isset($nested['#value'])) {
      switch ($nested['#type']) {
        case 'boolean':
          return (bool)$nested['#value'];
        case 'integer':
        case 'many2one':
          return (int)$nested['#value'];

        case 'float':
          return (float)$nested['#value'];

        case 'many2many':
          return null;

        case 'date':
          return false;

        default:
          return $nested['#value'];
      }
    }

    //render multi-value fields
    $vals = array();
    foreach ($nested as $key => $val) {
      if ($key === (string)(int)$key || strpos($key, '0-') === 0) {
        $resp = self::flatJsSend($val, $key);

        if (!is_null($resp)) {
          $vals[] = $resp;
        }
      }
      else {
        if (strpos($key, 'view[') === 0) {
          preg_match('/view\[(.*)\]/', $key, $m);
          $key = $m[1];
        }

        $vals[$key] = self::flatJsSend($val, $key);
      }
    }
    return $vals;
  }
}