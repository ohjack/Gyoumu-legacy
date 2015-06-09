<?php
namespace Gulei\Page\Form;

class Basic extends \Gulei\Page\_Abstract
{
  protected $q;
  protected $form_state;

  function __construct($form_state)
  {
    $this->q = $_GET['q'];
    $this->form_state = $form_state;
  }

  protected function getStateValues($array_parents = array(), $filter = false)
  {
    $vals = $this->form_state['values'];
    foreach($array_parents as $parent){
      $vals = $vals[$parent];
    }

    if(is_array($vals) && $filter){
      $ignore_pats = '/(^sel-|^form_|func:[^\]]*$)/';

      foreach(array_keys($vals) as $name){
        if(preg_match($ignore_pats, $name)){
          unset($vals[$name]);
        }
      }
    }
    return $vals;
  }

  protected function getFormTemplate()
  {
    return array('#tree' => true);
  }

  public function hook_form_submit($form, &$form_state)
  {
    if($callee = $this->getCallee()){
      $Call = new $callee($this);
      return $Call->execute($form, $form_state);
    }

    $btn = $this->getClickedButton();
    $this->{$btn->method_name}($form, $form_state);
  }

  protected function getCallee()
  {
    $name = $this->form_state['clicked_button']['#name'];

    if(preg_match('/^ns:(.*)/', $name, $m)){
      return str_replace(':', "\\", $m[1]);
    }
    return false;
  }

  protected function getClickedButton(){
    $btn = new \stdClass();
    $btn->element = $this->form_state['clicked_button'];

    $method_name = implode('_', $btn->element['#array_parents']);
    $method_name = preg_replace('/[[:punct:]]/', '_', $method_name);
    $method_name = 'proc_' . $method_name;
    $btn->method_name = $method_name;

    return $btn;
  }

  protected function getSelection($array_parents = array())
  {
    $sels = array();
    foreach($this->getStateValues($array_parents) as $name => $val){
      preg_match('/^sel-(.*)/', $name, $m);

      if($m[1] && $val){
        $sels[] = $m[1];
      }
    }
    return $sels;
  }

  public function goBack($reset = true, $anchors = array())
  {
    if($reset){
      $vals = array();
    }
    else{
      $vals = $_GET;
      unset($vals['q']);
    }

    drupal_goto($_GET['q'], $vals);
  }

  protected function getClickedButtonArrayParents($upLevel = 0, $downLevel = null)
  {
    $btn = $this->getClickedButton();
    $array_parents = $btn->element['#array_parents'];

    for($c = 1; $c <= $upLevel; $c++){
      array_pop($array_parents);
    }

    if(!is_null($downLevel)){
      $downs = is_string($downLevel)
          ? array($downLevel) : $downLevel;

      foreach($downs as $down){
        $array_parents[] = $down;
      }
    }
    return $array_parents;
  }

  protected function getValues()
  {
    return $this->form_state['values'];
  }

  protected function getDefaultValue($name, $default = '')
  {
    $vals = $this->form_state['values'];
    return isset($vals[$name]) ? $vals[$name] : $default;
  }
}