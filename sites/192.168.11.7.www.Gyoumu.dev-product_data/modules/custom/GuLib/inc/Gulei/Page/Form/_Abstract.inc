<?php
namespace Gulei\Page\Form;

abstract class _Abstract extends \Gulei\Page\_Abstract
{
  protected $q;
  protected $form_state;
  protected $args;

  abstract protected function getClassname();
  abstract protected function getParam();
  abstract protected function getToolbarBottom();
  abstract protected function hook_form_internal();

  public function __construct(&$form_state = null)
  {
    $this->redirect($form_state);

    $this->q = $_GET['q'];
    $this->form_state = &$form_state;
    $this->init();
  }

  protected function init() {}

  public function hook_form($args = array())
  {
    array_shift($args);
    $this->args = $args;

    $form = $this->hook_form_internal();
    $form['_classname_']  = array(
      '#type' => 'value',
      '#value' => $this->getClassname(),
    );

    if(!$form['#theme']){
      $form['#theme'] = 'GuleiPageForm';
    }

    $param = $this->getParam();

    if(!$param['hide'] || !in_array('toolbar-bottom', $param['hide'], true)){
      $form['toolbar-bottom'] = $this->getToolbarBottom();
      $form['toolbar-bottom']['#prefix'] = '<div class="toolbar-bottom">';
      $form['toolbar-bottom']['#suffix'] = '</div>';
    }
    return $form;
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

  public function hook_validate($form, &$form_state) {
    if($Callee = $this->getCallee()){
      $Callee->validate($form, $form_state);
    }
  }

  public function hook_form_submit($form, &$form_state)
  {
    if($Callee = $this->getCallee()){
      $rebuild = $Callee->execute($form, $form_state);
    }
    else{
      $btn = $this->getClickedButton();
      $rebuild = $this->{$btn->method_name}($form, $form_state);
    }

    if($redirect = $form_state['storage']['redirect']){
      $form_state['storage'] = array('redirect' => $redirect);
    }
    else if (!$rebuild){
      unset($form_state['storage']);
    }
  }

  protected function getCallee()
  {
    $btn = $this->form_state['clicked_button'];
    $name = $btn['#callee'] ? $btn['#callee'] : $btn['#name'];

    if(preg_match('/^ns:(.*)/', $name, $m)){
      $callee = str_replace(':', "\\", $m[1]);
      return new $callee($this, $btn);
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

  public function getSelection($array_parents = array())
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

  public function getValues()
  {
    return $this->form_state['values'];
  }

  protected function getDefaultValue($name, $default = '')
  {
    $vals = $this->form_state['values'];
    if($storage = $this->form_state['storage']['values']){
      $vals = $storage;
    }
    return isset($vals[$name]) ? $vals[$name] : $default;
  }

  public function getLocalCalleeEle($calls, $over = array())
  {
    if(!is_array($calls)){
      $calls = array($calls);
    }

    $form = array();
    $basename = $this->getClassname() . "\\Callee\\";

    foreach($calls as $call) {
      if(strpos($call, "\\") === false){
        $callee = $basename . $call;
      }
      else{
        $callee = $call;
      }

      $Call = new $callee($this);
      $form[$Call->getEleName()] = array_merge($Call->getEle(), $over);
    }
    return $form;
  }

  public function hook_theme($form)
  {
    $buf = '';
    $buf .= gulib_render($form, $this);
    $buf .= drupal_render($form);
    drupal_add_css(drupal_get_path('module', 'oerp') . '/oerp.css');
    return $buf;
  }

  protected function createEle($name, $type, $over = array())
  {
    $ele = array(
      '#type' => $type,
      '#default_value' => $this->getDefaultValue($name),
    );
    return array_merge($ele, $over);
  }
}
