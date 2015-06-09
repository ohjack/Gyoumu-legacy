<?php
namespace Gulei\Form\Element;

abstract class _Abstract extends \Gulei\Arch\Element
{
  protected $default;

  abstract public function getEntry();

  function __construct($arch, $default)
  {
    parent::__construct($arch);
    $this->default = $default;
  }

  protected function getParam()
  {
    return $this->getNodeValues('./param', $this->getArch());
  }

  protected function getParamDefault()
  {
    $param = $this->getParam();

    if(isset($param['default'])){
      $default_val = $param['default'];
    }
    else{
      $default_val = null;
    }
    return $default_val;
  }

  protected function getDefault()
  {
    $attr = $this->getArchAttributes();

    if (!is_null($this->default)) {
      $default_val = $this->default;
    }
    else if ($paramDefault = $this->getParamDefault()){
      $default_val = $paramDefault;
    }
    else if (isset($attr['value'])) {
      $default_val = $attr['value'];
    }
    else{
      $default_val = null;
    }
    return $default_val;
  }

  protected function getEntryAttributes()
  {
    $param = $this->getParam();
    $attrs = (array)json_decode($param['attrs']);
    $attrs = is_array(($attrs)) ? $attrs : array();

    if ($param['readonly'])
      $attrs['readonly'] = 'readonly';

    if ($param['confirm']) {
      if (isset($attrs['class'])) {
        $attrs['class'] .= ' needConfirm';
      }
      else {
        $attrs['class'] = 'needConfirm';
      }
    }
    return $attrs;
  }

  protected function getArchAttributes()
  {
    $attr = $this->attr($this->getArch());

    if (strpos($attr['value'], 'fn:') === 0) {
      preg_match('/fn:(.*)/', $attr['value'], $m);
      $attr['value'] = eval('return ' . $m[1] . ';');
    }

    return $attr;
  }

  protected function getEntryTemplate()
  {
    $param = $this->getParam();
    $attr = $this->getArchAttributes();

    $ele = array(
      '#title' => $attr['string'] ? $attr['string'] : $attr['name'],
      '#required' => $param['req'],
      '#description' => $param['desc'],
      '#type' => $attr['type'],
      '#default_value' => $this->getDefault(),
      '#attributes' => $this->getEntryAttributes(),
      '#ahah' => (array)json_decode($param['ahah']),
      '#prefix' => sprintf(
        '<div gu_type="%s" gu_name="%s" anchor="%s">%s',
        $attr['type'], $attr['name'], $attr['anchor'], $param['prefix']),

      '#suffix' => $param['suffix'] . '</div>',
    );

    return $ele;
  }
}