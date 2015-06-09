<?php
namespace Oerp\Product\Command;

class getFedexPackDim extends \Oerp\Command\WithStaticCache\_Abstract
{
  const KEY_L = 'packaging.length';
  const KEY_W = 'packaging.width';
  const KEY_H = 'packaging.height';
  const KEY_WT = 'packaging.weight';

  private $rec;

  protected function getCacheId()
  {
    return __CLASS__;
  }

  protected function setCache()
  {
    $arch = <<<XML
<tree>
  <param name="oerp:model">product.packaging</param>
  <field name="product_id">
    <field name="id"/>
  </field>
  <field name="ul">
    <field name="name"/>
    <field name="x_length"/>
    <field name="x_width"/>
    <field name="x_height"/>
  </field>
</tree>
XML;

    $q = new \Oerp\Query\Nested($arch);
    $recs = $q->sum();

    $cache = array();
    foreach($recs as $rec){
      $pid = $rec['product_id']['id'];
      $cache[$pid][] = $rec['ul'];
    }
    $this->setCacheInternal($cache);
  }

  private function getPackType($name)
  {
    preg_match('/^func:(.*)_/', $name, $m);
    return $m[1];
  }

  private function searchPack($pat)
  {
    $cache = $this->getCache();
    if(is_null($cache)){
      $this->setCache();
      $cache = $this->getCache();
    }

    $rec = $this->rec;
    $packs = $cache[$rec['id']];

    if(is_array($packs)){
      foreach($packs as $pack) {
        if(preg_match($pat, $pack['name'])){
          return $pack;
        }
      }
    }
    return false;
  }

  private function getPackOpt($name)
  {
    preg_match('/^func:(.*)_(.*)/', $name, $m);
    array_shift($m);

    return array('type' => $m[0], 'field' => $m[1]);
  }

  private function getValue()
  {
    $name = $this->ref->val('name');
    $opt = $this->getPackOpt($name);

    if($opt['type'] == 'fedex') {
        $pack = $this->searchPack('/^fedex /i');
        if(!$pack){
          $pack = $this->searchPack('/.*/');
        }
    }
    else{
      $pack = $this->searchPack('/.*/');
    }

    if(!$pack){
      return false;
    }

    switch($opt['field']){
      case 'pack':
        return $pack['name'];

      case 'wt':
        return $this->rec[self::KEY_WT];
    }

    if(stripos($pack['name'], 'original package') !== false){
      switch($opt['field']){
        case 'l':
          return $this->rec[self::KEY_L];

        case 'w':
          return $this->rec[self::KEY_W];

        case 'h':
          return $this->rec[self::KEY_H];
      }
    }
    else{
      switch($opt['field']){
        case 'l':
          return $pack['x_length'];

        case 'w':
          return $pack['x_width'];

        case 'h':
          return $pack['x_height'];
      }
    }
  }

  public function execute($refVar = null)
  {
    list($this->rec) = $this->ref->vals('rec');
    return $this->getValue();
  }
}