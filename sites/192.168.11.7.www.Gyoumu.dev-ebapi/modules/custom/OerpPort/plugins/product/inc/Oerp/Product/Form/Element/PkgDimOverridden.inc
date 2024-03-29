<?php
namespace Oerp\Product\Form\Element;

class PkgDimOverridden extends \Oerp\Form\Element\Float
{
  private function isOverridden()
  {
    $ul_id = is_array($tmp = $this->args['defaults']['ul'])
        ? $tmp[0] : $tmp;

    $q = new \Oerp\Query\Basic('product.ul', null, null, $ul_id);
    $rec = $q->raw()->offsetGet(0);

    return $rec['x_' . $this->args['name']];
  }

  public function getEntryTemplate()
  {
    $name = $this->args['name'];

    if($over = $this->isOverridden()){
      $this->args['default'] = 0;
      $this->args['defaults'][$name] = 0;
      $this->args['field']['readonly'] = true;
      $this->args['field']['attributes']['no-override'] = true;
    }
    return parent::getEntryTemplate();
  }

  public function prepareSend()
  {
    if($this->isOverridden()){
      return '';
    }
    else{
      parent::prepareSend();
    }
  }
}