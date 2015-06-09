<?php
namespace Oerp\Form\Element;

class Many2one extends _Abstract
{
  public function prepareSend()
  {
    $val = $this->args['value'];
    return is_array($val)
        ? (int)$val['rid']
        : (int)$val;
  }

  protected function getPrepareSendJs()
  {
    return <<<JS
var pat = /(.*)\[name\]/;
var name = pat.exec(fld['name'])[1];

return {
  '#name' : name,
  '#type' : 'many2one',
  '#value' : ele.attr('rid'),
  '#text' : fld['value']
};
JS;
  }

  protected function getEntryTemplate()
  {
    $args = $this->args;
    $dval = $args['default'];
    $rel = $args['field']['relation'];

    if (!is_null($dval)) {
      if (is_array($dval)) {
        $ele['#value'] = array(
          'id' => $dval[0],
          'name' => $dval[1],
        );
      }
      else {
        $q = new \Oerp\Query\Basic($rel, null, null, $dval);
        $resp = $q->name_get();

        $ele['#value'] = array(
          'id' => $resp[0][0],
          'name' => $resp[0][1],
        );
      }
    }

    $ele['#value']['model'] = $rel;
    $ele['#type'] = 'oerp_many2one';
    $ele['#attributes']['prepareSend'] = $this->getPrepareSendJs();

    if (!empty($args['field']['domain'])) {
      $ele['#attributes']['domain'] = json_encode($args['field']['domain']);
    }

    if($args['field']['selectonly']){
      $ele['#attributes']['selectonly'] = 1;
    }

    if(is_array($attrs = $this->args['field']['attributes'])){
      foreach($attrs as $attr_name => $attr_val){
        $ele['#attributes'][$attr_name] = $attr_val;
      }
    }

    if ($this->eleFld->getAttribute('invisible')) {
      $ele['#type'] = 'hidden';

      if(is_array($ele['#value'])){
        $ele['#value'] = $ele['#value']['id'];
      }
    }
    return $ele;
  }
}