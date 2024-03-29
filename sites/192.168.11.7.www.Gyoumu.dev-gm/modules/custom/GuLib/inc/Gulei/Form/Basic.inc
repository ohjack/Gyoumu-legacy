<?php
namespace Gulei\Form;

class Basic extends \Gulei\Arch\Element
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
    $Classname = "Gulei\\Form\\Element\\" . ucfirst($type);

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
    $arch = new \Gulei\Arch\Element(
      '<root>' . str_replace('&', '&amp;', $form['#children']) . '</root>');

    $F = new \Gulei\Arch\Composer($this->dump());
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