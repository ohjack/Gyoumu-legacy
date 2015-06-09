<?php
namespace Oerp\Form\Element;

class SkuEditor extends _Abstract
{
  public function prepareSend()
  {
    $val = (array)$this->args['value'];

    if($val['sn'] === 'XXXXX'){
      $q = new \Oerp\Query\Basic('product.product', null, array('default_code'));
      $skus = $q->raw()->collect('default_code');

      $tail = 0;
      foreach($skus as $sku){
        $toks = explode('-', $sku);
        if(count($toks) == 2 && is_numeric($toks[1]) && $toks[1] > $tail){
          $tail = (int)$toks[1];
        }
      }
      $val['sn'] = sprintf('%05s', $tail + 1);
    }

    return strtoupper($val['code']) . '-' . $val['sn'];
  }

  protected function getRetrieveValueJs()
  {
    return <<<JS
var pat = /(.*)\[holder\]/;
var name = pat.exec(ele.attr('name'))[1];
dialog.find('*[name="' + name + '[code]"]').val(values[name]['#value']['code']);
dialog.find('*[name="' + name + '[sn]"]').val(values[name]['#value']['sn']);
JS;
  }

  protected function getPrepareSendJs()
  {
    return <<<JS
var pat = /(.*)\[holder\]/;
var name = pat.exec(ele.attr('name'))[1];
var code = form.find('*[name="' + name + '[code]"]').val().toUpperCase();
var sn = form.find('*[name="' + name + '[sn]"]').val();

return {
  '#name' : name,
  '#type' : 'skuEditor',
  '#value' : {
    'code' : code,
    'sn' : sn
  },
  '#text' : code + '-' + sn
};
JS;
  }

  protected function getEntryTemplate()
  {
    $default = (string)$this->args['default'];

    if(empty($default)){
      $skuToks = array('', 'XXXXX');
    }
    else{
      $skuToks = explode('-', $default);
    }

    $ele = array(
      'code' => array(
        '#title' => $this->getCaption(),
        '#description' => $this->args['field']['help'],
        '#type' => 'textfield',
        '#size' => '',
        '#maxlength' => 2,
        '#default_value' => $skuToks[0],
        '#required' => $this->args['field']['required'] ? true : false,
        '#attributes' => array('style' => 'width:3em;'),
        '#prefix' => '<div style="float:left; width: 4em;">',
        '#suffix' => '</div>',
      ),
      'sn' => array(
        '#title' => '<sup> </sup>',
        '#type' => 'textfield',
        '#size' => '',
        '#default_value' => $skuToks[1],
        '#attributes' => array(
          'style' => 'width:100%;',
          'readonly' => 'readonly'
        ),
        '#prefix' => '<div style="float:left; width: 75%;">',
        '#suffix' => '</div>',
      ),
      'holder' => array(
        '#type' => 'hidden',
        '#value' => '',
        '#attributes' => array(
          'prepareSend' => $this->getPrepareSendJs(),
          'retrieve' => $this->getRetrieveValueJs(),
        ),
      ),
      '#prefix' => '<div id="edit-view-default-code-wrapper">',
      '#suffix' => '<div style="clear:both;"></div></div>',
    );
    return $ele;
  }
}