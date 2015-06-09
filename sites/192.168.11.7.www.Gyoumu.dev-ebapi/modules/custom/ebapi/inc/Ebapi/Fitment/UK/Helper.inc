<?php
namespace Ebapi\Fitment\UK;

class Helper extends \Ebapi\Fitment\AbstractHelper
{
  protected function parseId($id) {
    preg_match('/^(\d+)-(\d+)-(\d+)/', $id, $m);
    return $m;
  }

  public function queryById($id)
  {
    $toks = $this->parseId($id);
    $sql = 'SELECT * FROM {ebapi_uk_fitment} WHERE `Ktype`=%d LIMIT 1';
    $rs = db_query($sql, $toks[1]);
    return db_fetch_object($rs);
  }

  protected function getQuery($cri) {
    return new \Ebapi\Query\UK\Fitment($cri, null);
  }

  public function addCompaNote($id, $rec) {
    $toks = $this->parseId($id);
    $rec['CompatibilityNotes'] =
        sprintf('Only for %s~%s', $toks[2], $toks[3]);
    return $rec;
  }
}