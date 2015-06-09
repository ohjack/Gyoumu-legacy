<?php
namespace Oerp\Form\Element;

abstract class _Abstract
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

    $arch = new \Gulei\Arch\Element($this->args['arch']);
    $this->eleFld = $arch->getArch();
  }

  protected function getCaption()
  {
    $args = $this->args;
    list($fld, $arch) = array($args['field'], $args['arch']);

    $arch = new \Gulei\Arch\Element($arch);
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
    $ele = $this->getEntryTemplate();
    $args = $this->args;

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
    foreach (\Oerp\View\Form::getPassAttrs() as $pass_attr) {
      if ($this->eleFld->getAttribute($pass_attr)) {
        $ele['#attributes'][$pass_attr] = $this->eleFld->getAttribute($pass_attr);
      }
    }

    if(is_array($attrs = $this->args['field']['attributes'])){
      foreach($attrs as $attr_name => $attr_val){
        $ele['#attributes'][$attr_name] = $attr_val;
      }
    }
    return $ele;
  }
}
