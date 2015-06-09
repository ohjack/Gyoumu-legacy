<?php
namespace Oerp\Purchase\Provider;

class PoLines extends \Gulei\Provider\_Abstract {
  protected $ids;
  protected $flags;

  public function __construct($ids, $flags = array()) {
    $this->ids = is_array($ids) ? $ids : array($ids);
    $this->flags = $flags;
  }

  public function getArch() {
    return file_get_contents(__DIR__ . '/PoLines.xml');
  }

  public function getData() {
    if (!$this->ids) {
      return array();
    }

    $arch = $this->getArch();

    $param = array(
      'oerp:ids' => $this->ids
    );

    $flag = array(
      'no-selection-name' => TRUE,
      'no-cache' => TRUE,
    );

    $flag = array_merge($flag, $this->flags);

    $q = new \Oerp\Query\Nested($arch, $param);
    $recs = $q->sum($flag);

    $funcASort = create_function('$key1,$key2', '
      $pat = "/..-(.*)/";
      preg_match($pat, $key1, $m1);
      preg_match($pat, $key2, $m2);
      return $m1[1] > $m2[1];
    ');

    foreach ($recs as &$rec) {
      $lines = array();

      foreach ($rec['order_line'] as $line) {
        $lines[$line['product_id']['default_code']] = $line;
      }

      uksort($lines, $funcASort);
      $rec['order_line'] = $lines;
    }
    return $recs;
  }

  public function count() {
    return count($this->getData());
  }

  public function getReference() {
    return array(
      'ids' => $this->ids,
    );
  }
}