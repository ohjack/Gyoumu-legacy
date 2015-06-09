<?php
namespace Oerp\Stock\Page;

class ReviseRop extends \Gulei\Page\Form\_Abstract
{
  const MSR_MODEL = 'stock.warehouse.orderpoint';
  const FLD_MIN = 'product_min_qty';
  const FLD_MAX = 'product_max_qty';
  const FLD_UOM = 'product_uom';

  static private $rules;

  protected function getClassname()
  {
    return __CLASS__;
  }

  private function getSales()
  {
    $cri = array(
      array('state', '<>', 'draft'),
      array('state', '<>', 'obsolete'),
    );
    
    $Prov = new \Oerp\Stock\Provider\SafetyStock(null, $cri);
    return (array)$Prov->getData();
  }

  private function initRules($pids)
  {
    if(is_null(self::$rules)){
      $q = new \Oerp\Query\Basic(
        self::MSR_MODEL,
        array(array('product_id', 'in', $pids)),
        array(
          'id',
          'product_id',
          'product_min_qty',
          'product_max_qty',
          'product_uom',
        )
      );
      self::$rules = $q->raw()->promote('product_id');
    }
  }

  private function revise($sale)
  {
    $pid = $sale['id'];
    $uom_id = $sale['uom_id'][0];
    $min = $max = $sale['sum'];

    $q = new \Oerp\Query\Basic(self::MSR_MODEL);

    if($rule = self::$rules[$pid]){
      if($rule[self::FLD_MIN !== $min]
        || $rule[self::FLD_MAX] !== $max
        || $rule[self::FLD_UOM][0] !== $uom_id
      ){
        $q->setIds($rule['id']);
        return $q->write(array(
          'product_uom' => $uom_id,
          'product_min_qty' => $min,
          'product_max_qty' => $max
        ));
      }
    }
    else{
      return $q->create(array(
        'product_id' => $pid,
        'product_uom' => $uom_id,
        'product_min_qty' => $min,
        'product_max_qty' => $max,
      ));
    }
  }

  protected function hook_form_internal()
  {
    $sales = $this->getSales();
    $this->initRules(array_keys($sales));

    foreach($sales as $sale){
      $this->revise($sale);
    }
    return array();
  }

  protected function getParam()
  {
    return array();
  }

  protected function getToolbarBottom()
  {
    return array();
  }
}