<?php
namespace Oerp;

module_load_include('inc', 'GuLib', 'include/GuLib');
module_load_include('inc', 'GuLib', 'include/GuArch');
require_once('query.inc');

abstract class AbstractFormElement
{
  protected $args;
  protected $eleFld;

  abstract public function prepareSend();
  abstract protected function getEntryTemplate();
  abstract protected function getPrepareSendJs();

  public function __construct($args)
  {
    $this->args = $args;
    $this->initArgs();
  }

  private function initArgs()
  {
    $args = &$this->args;

    //overrids state-sensitive fields
    if($args['field']['states']
       && $state_overrides = $args['field']['states'][$args['state']]){

      foreach($state_overrides as $value => $params){
        foreach($params as $param){
          if($param){
            $args['field'][$param] = $value;
          }
        }
      }
    }
  }

  private function initArch()
  {
    if(!$this->args['arch']){
      echo '<pre>';
      print_r($this->args);
      throw new \Exception('arch not provided');
    }

    $arch = new \GuArchElement($this->args['arch']);
    $this->eleFld = $arch->getArch();
  }

  protected function getCaption()
  {
    $args = $this->args;
    list($fld, $arch) = array($args['field'], $args['arch']);

    $arch = new \GuArchElement($arch);
    $eleFld = $arch->getArch();

    if ($eleFld->getAttribute('nolabel')) {
      $caption = '';
    }
    else {
      $caption = $eleFld->getAttribute('string');
      $caption = $caption ? $caption : $fld['string'];
      $caption = htmlspecialchars($caption);
      $caption = $fld['help'] ? '<sup>[?]</sup>' . $caption : '<sup> </sup>' . $caption;
    }

    return $caption;
  }

  public function getEntry()
  {
    $this->initArch();
    $args = $this->args;
    $ele = $this->getEntryTemplate();
    $ele['#title'] = $this->getCaption();
    $ele['#description'] = $args['field']['help'];
    $ele['#required'] = $args['field']['required'] ? true : false;

    if ($this->eleFld->getAttribute('invisible')) {
      $ele['#type'] = 'hidden';
    }

    if ($args['field']['readonly']) {
      switch ($ele['#type']) {
        case 'select':
          $ele['#attributes']['disabled'] = 'disabled';
          break;

        default:
          $ele['#attributes']['readonly'] = 'readonly';
          break;
      }
    }

    //Handle attributes which should be passed
    foreach (\OerpFormView::getPassAttrs() as $pass_attr) {
      if ($this->eleFld->getAttribute($pass_attr)) {
        $ele['#attributes'][$pass_attr] = $this->eleFld->getAttribute($pass_attr);
      }
    }

    return $ele;
  }
}

class FormElementText extends AbstractFormElement
{
  public function prepareSend()
  {
    return (string)$this->args['value'];
  }

  protected function getPrepareSendJs()
  {
    return <<<JS
return {
  '#name' : fld['name'],
  '#value' : fld['value'],
  '#type' : 'text'
};
JS;
  }

  protected function getEntryTemplate()
  {
    $text = (string)$this->args['default'];
    $ele['#type'] = 'textarea';
    $ele['#default_value'] = $text;
    $ele['#attributes'] = array('prepareSend' => $this->getPrepareSendJs());
    return $ele;
  }
}

class FormElementSelection extends AbstractFormElement
{
  public function prepareSend()
  {
    return (string)$this->args['value'];
  }

  protected function getPrepareSendJs()
  {
    return <<<JS
var ele_txt = ele
  .children('option[value="' + fld['value'] + '"]').eq(0)
  .text();

return {
  '#name' : fld['name'],
  '#text' : ele_txt,
  '#value' : fld['value'],
  '#type' : 'selection'
};
JS;
  }

  protected function getEntryTemplate()
  {
    $ele['#type'] = 'select';
    $opts = array();
    foreach ($this->args['field']['selection'] as $opt) {
      $opts[$opt[0]] = $opt[1];
    }

    $ele['#options'] = $opts;
    $ele['#default_value'] = $this->args['default'];
    $ele['#attributes'] = array('prepareSend' => $this->getPrepareSendJs());
    return $ele;
  }
}

class FormElementBoolean extends AbstractFormElement
{
  public function prepareSend()
  {
    return (bool)$this->args['value'];
  }

  protected function getPrepareSendJs()
  {
    return <<<JS
return {
  '#name' : fld['name'],
  '#type' : 'boolean',
  '#value' : fld['value']
};
JS;
  }

  protected function getEntryTemplate()
  {
    $ele['#type'] = 'checkbox';
    $ele['#default_value'] = $this->args['default'];
    $ele['#attributes'] = array('prepareSend' => $this->getPrepareSendJs());
    return $ele;
  }
}

abstract class AbstractFormElementDateLike extends AbstractFormElement
{
  public function prepareSend()
  {
    $val = (string)$this->args['value'];
    return strlen(trim($val)) ? $val : false;
  }
}

class FormElementDate extends AbstractFormElementDateLike
{
  protected function getPrepareSendJs()
  {
    return <<<JS
return {
  '#name' : fld['name'],
  '#type' : 'date',
  '#value' : fld['value']
};
JS;
  }

  protected function getEntryTemplate()
  {
    $ele['#type'] = 'textfield';
    $ele['#default_value'] = $this->args['default'];
    $ele['#attributes']['widget'] = 'datepicker';
    $ele['#attributes']['prepareSend'] = $this->getPrepareSendJs();
    return $ele;
  }
}

class FormElementDatetime extends AbstractFormElementDateLike
{
  protected function getPrepareSendJs()
  {
    return <<<JS
return {
  '#name' : fld['name'],
  '#type' : 'datetime',
  '#value' : fld['value']
};
JS;
  }

  protected function getEntryTemplate()
  {
    $ele['#type'] = 'textfield';
    $ele['#default_value'] = $this->args['default'];
    $ele['#attributes']['widget'] = 'datetimepicker';
    $ele['#attributes']['prepareSend'] = $this->getPrepareSendJs();
    return $ele;
  }
}

class FormElementMany2one extends AbstractFormElement
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
        $q = new \OerpQuery($rel, null, null, $dval);
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

    if ($this->eleFld->getAttribute('invisible')) {
      $ele['#type'] = 'hidden';

      if(is_array($ele['#value'])){
        $ele['#value'] = $ele['#value']['id'];
      }
    }
    return $ele;
  }
}

class FormElementOne2Many extends AbstractFormElement
{
  public function prepareSend()
  {
    return $this->args['nested_value'];
  }

  protected function getPrepareSendJs()
  {
    return <<<JS
return {
  '#name' : fld['name'],
  '#type' : 'one2many',
  '#value' : fld['value']
};
JS;

  }

  protected function getEntryTemplate()
  {
    $args = $this->args;
    $fld = $args['field'];

//        get ids of @fld_name for this record
    $q = new \OerpQuery(
      $args['model'],
      null,
      array($args['name']),
      array($args['rid'])
    );

    $resp = $q->raw();
    $ids = $resp[0][$args['name']];

    if (empty($ids)) {
      $ids = array(0);
    }

//        build element 'oerp_treeview'
    $ele['#type'] = 'oerp_treeview';

    $param = array(
      'oerp:model' => $fld['relation'],
      'fields' => $fld['views']['tree']['fields'],
      'oerp:ids' => $ids,
      'name' => $args['name'],
      'arch' => $fld['views']['tree']['arch'],
      'caption' => $this->getCaption(),
      'key' => sprintf('%s-%s', $args['key'], str_replace('_', '-', $args['name'])),
      'oerp:op-type' => 'edit',
      'oerp:type' => 'one2many',
      'escape' => true,
    );

    if (isset($fld['views']['form'])) {
      $cid = sprintf('formview:%s-%s', $param['model'], $param['name']);
      cache_set($cid, $fld['views']['form'], 'cache_oerpport', CACHE_TEMPORARY);
      $param['cache_id'] = $cid;
    }

    $ele['#param'] = $param;

//        HTML snippet anchor
    $ele['#prefix'] =
        '<div id="edit-' . $param['key'] . '-wrapper">';

    $ele['#suffix'] = '</div>';
    return $ele;
  }
}

abstract class AbstractFormElementTextfieldLike extends AbstractFormElement
{
  public function prepareSend()
  {
    return (string)$this->args['value'];
  }

  protected function getEntryTemplate()
  {
    $ele['#type'] = 'textfield';
    $ele['#size'] = '';
    $ele['#default_value'] = (string)$this->args['default'];
    $ele['#attributes'] = array('prepareSend' => $this->getPrepareSendJs());
    return $ele;
  }
}

class FormElementChar extends AbstractFormElementTextfieldLike {
  protected function getPrepareSendJs()
  {
    return <<<JS
return {
  '#name' : fld['name'],
  '#type' : 'char',
  '#value' : fld['value']
};
JS;
  }
}

class FormElementFloat extends AbstractFormElementTextfieldLike {
  protected function getPrepareSendJs()
  {
    return <<<JS
return {
  '#name' : fld['name'],
  '#type' : 'float',
  '#value' : fld['value']
};
JS;
  }
}

class FormElementInteger extends AbstractFormElementTextfieldLike {
  protected function getPrepareSendJs()
  {
    return <<<JS
return {
  '#name' : fld['name'],
  '#type' : 'integer',
  '#value' : fld['value']
};
JS;
  }
}

class FormElementFallback extends AbstractFormElement
{
  public function prepareSend()
  {
    return null;
  }

  protected function getPrepareSendJs()
  {
    return <<<JS
return {
  '#name' : fld['name'],
  '#type' : 'fallback',
  '#value' : fld['value']
};
JS;

  }

  protected function getEntryTemplate()
  {
    $ele['#type'] = 'textfield';
    $ele['#default_value'] = $this->args['field']['type'];
    $ele['#attributes']['readonly'] = 'readonly';
    return $ele;
  }
}

class FormElementSkuEditor extends AbstractFormElement
{
  public function prepareSend()
  {
    $val = (array)$this->args['value'];

    if($val['sn'] === 'XXXXX'){
      $q = new \OerpQuery('product.product', null, array('default_code'));
      $skus = $q->raw()->collect('default_code');

      $tail = 0;
      foreach($skus as $sku){
        $toks = explode('-', $sku);
        if(count($toks) == 2 && is_numeric($toks[1]) && $toks[1] > $tail){
          $tail = (int)$toks[1];
        }
      }
      $val['sn'] = $tail + 1;
    }

    return $val['code'] . '-' . $val['sn'];
  }

  protected function getPrepareSendJs()
  {
    return <<<JS
var pat = /(.*)\[holder\]/;
var name = pat.exec(ele.attr('name'))[1];
var code = form.find('*[name="' + name + '[code]"]').val();
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
        '#attributes' => array('prepareSend' => $this->getPrepareSendJs())
      ),
      '#prefix' => '<div id="edit-view-default-code-wrapper">',
      '#suffix' => '<div style="clear:both;"></div></div>',
    );
    return $ele;
  }
}

class FormElementFactory
{
  static public function createElement($args)
  {
    $type = $args['field']['type'];
    $classname = 'Oerp\FormElement' . ucfirst($type);

    if(class_exists($classname)){
      $Ele = new $classname($args);
    }
    else{
      $Ele = new FormElementFallback($args);
    }
    return $Ele;
  }

  static public function prepareSend($args)
  {
    $Ele = self::createElement($args);
    return $Ele->prepareSend();
  }
}