<?php 

module_load_include('inc', 'GuLib', 'include/GuTable');

abstract class AbstractFormElement extends GuArchElement
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

class FormEleTextarea extends AbstractFormElement
{
  public function getEntry()
  {
    return $this->getEntryTemplate();
  }
}

class FormEleTextfield extends AbstractFormElement
{
  public function getEntry()
  {
    $tpl = $this->getEntryTemplate();
    if (!isset($this->param['size'])) {
      $tpl['#size'] = '';
    }
    return $tpl;
  }
}

class FormEleFile extends AbstractFormElement
{
  public function getEntry()
  {
    $tpl = $this->getEntryTemplate();
    $tpl['#size'] = '';
    return $tpl;
  }
}

class FormEleHidden extends AbstractFormElement
{
  public function getEntry()
  {
    $attr = $this->getArchAttributes();
    $tpl = $this->getEntryTemplate();
    $tpl['#value'] = $attr['value'];
    return $tpl;
  }
}

class FormEleMarkup extends AbstractFormElement
{
  public function getEntry()
  {
    $tpl = $this->getEntryTemplate();
    $val = $this->xp('./value[1]')->item(0);
    $nodeVal = $val ? $val->nodeValue : '';

    $tpl['#value'] = is_null($default_val = $tpl['#default_value'])
        ? $nodeVal : $default_val;

    return $tpl;
  }
}

abstract class AbstractFormElementButtonLike extends AbstractFormElement
{
  protected function getEntryTemplate()
  {
    $attr = $this->getArchAttributes();
    $tpl = parent::getEntryTemplate();
    $tpl['#value'] = $attr['string'];
    $tpl['#name'] = $attr['name'];
    return $tpl;
  }
}

class FormEleButton extends AbstractFormElementButtonLike
{
  public function getEntry()
  {
    return $this->getEntryTemplate();
  }
}

class FormEleSubmit extends AbstractFormElementButtonLike
{
  public function getEntry()
  {
    return $this->getEntryTemplate();
  }
}

abstract class AbstractFormElementWithOption extends AbstractFormElement
{
  protected function getParamDefault()
  {
    $paramDefault = parent::getParamDefault();
    
    if($paramDefault){
      return (array)json_decode($paramDefault);
    }
    else{
      return null;
    }
  }

  protected function getEntryTemplate()
  {
    $tpl = parent::getEntryTemplate();
    $opts = array();
    
    foreach ($this->xp('./option') as $ndOpt) {
      $attOpt = $ndOpt->attributes;
      $opts[$attOpt->getNamedItem('value')->value] = $ndOpt->nodeValue;
    }

    $tpl['#options'] = $opts;

    if ($tpl['#attributes']['readonly']) {
      $tpl['#attributes']['disabled'] = 'disabled';
      unset($tpl['#attributes']['readonly']);
    }
    return $tpl;
  }
}

class FormEleSelect extends AbstractFormElementWithOption
{
  public function getEntry()
  {
    return $this->getEntryTemplate();
  }
}

class FormEleCheckboxes extends AbstractFormElementWithOption
{
  public function getEntry()
  {
    return $this->getEntryTemplate();
  }
}

class FormEleGuCsv extends AbstractFormElement
{
  public function getEntry()
  {
    $tpl = $this->getEntryTemplate();
    $tpl['#arch'] = $this->nodeValue;
    return $tpl;
  }
}

class GuForm extends GuArchElement
{
  private $defaults;

  function __construct($arch, $defaults = null)
  {
    parent::__construct($arch);
    $this->mapEles();
    $this->defaults = $defaults;
  }

  protected function getDefault($fld)
  {
    return $fld->getAttribute('value');
  }

  protected function mapEle($fld)
  {
    if ($fld->getAttribute('mapped')) {
      return;
    }

    switch ($fld->nodeName) {
      case 'tree':
        break;

      case 'field':
        if (!$name = $fld->getAttribute('name')) {
          return;
        }

        if ($fld->hasChildNodes()) {
          foreach ($this->xp('.//field', $fld) as $inn_fld) {
            if ($anchor = $inn_fld->getAttribute('anchor')) {
              $inn_fld->setAttribute('anchor', $anchor . '.' . $name);
            }
            else {
              $inn_fld->setAttribute('anchor', $name);
            }
          }
        }
    }

    $fld->setAttribute('mapped', '1');
  }

  protected function mapEles()
  {
    $this->walk(array($this, 'mapEle'));
  }

  protected function getEle($fld, &$form)
  {
    if ($fld->tagName == 'tree') {
      return;
    }

    $type = $fld->getAttribute('type');
    $name = $fld->getAttribute('name');
    $Classname = 'FormEle' . ucfirst($type);

    $Ele = new $Classname($this->dump($fld), $this->defaults[$name]);
    $form[$name] = $Ele->getEntry();
  }

  public function getFormEles($node = null, &$form = null)
  {
    if (is_null($node)) {
      $node = $this->getArch();
    }

    if (is_null($form)) {
      $form = array();
      $form['#obj'] = $this->save();
      $form['#tree'] = true;
    }

    foreach ($this->xp('./*', $node) as $child) {
      switch ($child->tagName) {
        case 'field':
          if ($this->xp('.//field', $child)->length) {
            $this->getInnerForm($child, $form);
          }
          else {
            $this->getEle($child, $form);
          }
          break;

        case 'tree':
          $this->getEle($child, $form);
          break;

        default:
          if ($this->xp('.//field', $child)->length) {
            $this->getFormEles($child, $form);
          }
      }
    }
    return $form;
  }

  protected function getInnerForm($fld, &$form)
  {
    $doc = new DOMDocument();
    $elForm = $doc->createElement('form');

    foreach ($fld->childNodes as $chd) {
      $elForm->appendChild($doc->importNode($chd, true));
    }
    $doc->appendChild($elForm);

    $inner_form = new GuForm($doc);
    $form[$fld->getAttribute('name')] = $inner_form->getFormEles();
  }

  public function getFormEntry($name = null)
  {
    $ele = array(
      '#type' => 'GuForm',
      '#obj' => $this->save(),
    );

    if (is_null($name)) {
      return $ele;
    }
    else {
      return array($name => $ele);
    }
  }

  public function getHtml()
  {
    $eles = $this->getFormEles();
    $eles['#obj'] = $this;
    return drupal_get_form('GuLib_GuForm_form', $eles);
  }

  public function theme(&$form)
  {
    $arch = new GuArchElement(
      '<root>' . str_replace('&', '&amp;', $form['#children']) . '</root>');

    $F = new GuArch($this->dump());
    $output = $F->renderForm($arch);

    foreach ($arch->xp('//input[@type="hidden"]') as $hidden) {
      $output .= $arch->dump($hidden);
    }
    return $output;
  }

  static public function addJs()
  {
    jquery_ui_add(array('ui.draggable', 'ui.dialog', 'ui.tabs', 'ui.datepicker', 'ui.slider'));
    drupal_add_js(drupal_get_path('module', 'GuLib') . '/js/jquery-ui-timepicker-addon.js');
  }

  static public function addCss()
  {
    drupal_add_css(drupal_get_path('module', 'GuLib') . '/GuLib.css');
    drupal_add_css(drupal_get_path('module', 'GuLib') . '/js/jquery-ui-timepicker-addon.css');
  }

  private function getFieldNode($name)
  {
    return $this->xp('//field[@name="' . $name . '"][1]')->item(0);
  }
}

function GuLib_GuForm_process($ele)
{
  $ele['#tree'] = true;
  $obj = $ele['#obj']->renew();

  return array_merge($obj->getFormEles(), $ele);
}

function theme_GuForm($form)
{
  GuForm::addCss();
  GuForm::addJs();

  $obj = $form['#obj']->renew();

  $output = sprintf('<div class="gu-view gu-block-%s">%s</div>',
                    $form['#width'], $obj->theme($form));

  return html_entity_decode($output);
}

?>