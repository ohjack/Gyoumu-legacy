<?php
namespace Oerp\Util;

class Helper
{
  static public function jsondecode($data)
  {
    $data = preg_replace("/%u([0-9a-f]{3,4})/i", "&#x\\1;", urldecode($data));
    $data = html_entity_decode($data, null, 'UTF-8');
    $data = json_decode($data);
    return $data;
  }

  static public function renderArrayInput($input)
  {
    $vals = array();
    for ($c = 0; $c < count($input); $c++) {
      foreach ($input[$c] as $key => $val) {
        $tok = explode('.', $key);

        if (count($tok) > 1) {
          $vals[$c][$tok[0]][$tok[1]] = trim($val);
        }
        else {
          if ($key == 'name') {
            $val = implode(' ', array_map('trim', explode("\n", $val)));
          }
          $vals[$c][$tok[0]] = trim($val);
        }
      }
    }

    return $vals;
  }

  static public function renderNestedInput($nested, $id = null)
  {
    if (!is_array($nested) && !is_object($nested)) {
      return null;
    }

    $nested = (array)$nested;

    //render records
    if ($id === (string)(int)$id || strpos($id, '0-') === 0) {
      //  records to delete
      if (isset($nested['#delete'])) {
        return ($id == 0)
            ? null
            : array(2, (int)$id, false);
      }

      return ($id == 0)
          ? array(0, 0, self::renderNestedInput($nested['#value']))
          : array(1, (int)$id, self::renderNestedInput($nested['#value']));
    }

    //render single-value fields
    if (isset($nested['#value'])) {
      $args = array(
        'field' => array('type' => $nested['#type']),
        'value' => $nested['#value'],
      );
      return \Oerp\Form\Element\Factory::prepareSend($args);
    }

    //render multi-value fields
    $vals = array();
    foreach ($nested as $key => $val) {
      if ($key === (string)(int)$key || strpos($key, '0-') === 0) {
        $resp = self::renderNestedInput($val, $key);

        if (!is_null($resp)) {
          $vals[] = $resp;
        }
      }
      else {
        if (strpos($key, 'view[') === 0) {
          preg_match('/view\[(.*)\]/', $key, $m);
          $key = $m[1];
        }

        $vals[$key] = self::renderNestedInput($val, $key);
      }
    }
    return $vals;
  }

  static public function addCss()
  {
    drupal_add_css(drupal_get_path('module', 'oerp') . '/oerp.css');
  }

  static public function addJs()
  {
    drupal_add_js(drupal_get_path('module', 'oerp') . '/oerp.js');

    foreach (func_get_args() as $file) {
      drupal_add_js(drupal_get_path('module', 'oerp') . '/' . $file . '.js');
    }
  }
}