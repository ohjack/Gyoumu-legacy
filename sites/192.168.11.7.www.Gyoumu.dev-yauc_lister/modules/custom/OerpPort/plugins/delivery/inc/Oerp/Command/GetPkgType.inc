<?php
namespace Oerp\Command;

class GetPkgType extends \Gulei\Command\WithReference\WithTable {
  protected static $GetPkgType;

  protected function getRecs() {
    list($recs) = $this->ref->vals('recs');
    return $recs;
  }

  protected function getSkus() {
    $skus = \Gulei\_Array::lambda(
      $this->getRecs(), 'collect', 'move_lines.product_id.default_code');
    return $skus;
  }

  protected function getIds() {
    $ids = \Gulei\_Array::lambda($this->getRecs(), 'collect', 'id');
    return array_values(array_filter($ids));
  }

  public function execute($refVar = NULL) {
    if (self::$GetPkgType === NULL) {
      self::$GetPkgType = new \Oerp\Delivery\Helper\GetPkgType(
        $this->getSkus(), $this->getIds());
    }

    list($rec) = $this->ref->vals('rec');
    $pkg = self::$GetPkgType->get($rec['id']);
    return $pkg['name'];
  }
}