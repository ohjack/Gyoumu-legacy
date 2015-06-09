<?php
namespace Oerp\Arch;

class Form extends \Gulei\Form\Basic
{
  private $fields;
  private $defaults;

  private static $TYPE_MAP = array(
    'char' => 'textfield',
    'float' => 'textfield',
    'integer' => 'textfield',
    'boolean' => 'checkbox',
    'date' => 'textfield',
    'datetime' => 'textfield',
    'text' => 'textarea',
    'selection' => 'select',
    'many2one' => 'select',
    'one2many' => 'form',
  );

  function __construct($arch, $parent_model = null)
  {
    parent::__construct($arch);
    $this->mapEles();
  }

  protected function getFieldDef($mod = null)
  {
    if (!$mod) {
      return $this->fields;
    }
    else {
      if (!$this->fields[$mod]) {
        $this->fields[$mod] = (array)\Oerp\Query\Fields::getDef($mod);

        return $this->fields[$mod];
      }
      else {
        return $this->fields[$mod];
      }
    }
  }

  private function defaults($mod = null, $fld = null)
  {
    if (is_null($mod) && is_null($fld)) {
      $models = array();
      foreach ($this->xp('//@model') as $ele) {
        $models[] = $ele->value;
      }

      $models = array_unique($models);

      foreach ($models as $model) {
        if ($model) {
          $q = new \Oerp\Query\Fields($model);
          $this->defaults[$model] = $q->defaults();
        }
      }
      return $this->defaults;
    }

    if (!is_null($mod)) {
      if (!isset($this->defaults[$mod])) {
        $q = new \Oerp\Query\Fields($mod);
        $this->defaults[$mod] = $q->defaults();
      }

      return is_null($fld)
          ? $this->defaults[$mod]
          : $this->defaults[$mod][$fld];
    }
    return null;
  }

  protected function getDefault($fld)
  {
    $default_val = $this->defaults(
      $fld->getAttribute('model'), $fld->getAttribute('name'));

    return $default_val;
  }

  protected function getEle($fld, &$form)
  {
    if ($fld->tagName == 'tree') {
      $t = new \Oerp\Table\Basic($this->dump($fld));
      $form[$fld->getAttribute('name')] = $t->getFormEles();
    }
    else {
      parent::getEle($fld, $form);
    }
  }

  protected function mapEle($fld)
  {
    parent::mapEle($fld);

    if ($fld->getAttribute('mapped') >= 2) {
      return;
    }

    $model = $fld->getAttribute('model');

    switch ($fld->nodeName) {
      case 'tree':
        break;

      case 'form':
        foreach ($this->xp('.//field', $fld) as $inn_fld) {
          if ($this->xp('./ancestor::tree[1]', $inn_fld)->length) {
            continue;
          }

          if ($fld->hasAttribute('relation')) {
            $inn_fld->setAttribute('model', $fld->getAttribute('relation'));
          }
          else {
            $inn_fld->setAttribute('model', $model);
          }
        }
        break;

      case 'field':
        if ($this->xp('./ancestor::tree[1]', $fld)->length) {
          continue;
        }

        $fldDef = \Oerp\Query\Fields::getDef($model);

        if (!$name = $fld->getAttribute('name')) {
          return;
        }

        if ($fldDef[$name]['relation'] && $fldDef[$name]['relation'] != 'NULL') {
          $fld->setAttribute('relation', $fldDef[$name]['relation']);
        }

        if (!$fld->getAttribute('string')) {
          $fld->setAttribute('string',
                             ($fldDef[$name]) ? $fldDef[$name]['string'] : $name);
        }

        if ($fld->hasChildNodes()) {
          foreach ($this->xp('.//field', $fld) as $inn_fld) {
            if ($fld->hasAttribute('relation')) {
              $inn_fld->setAttribute('model', $fld->getAttribute('relation'));
            }
            else {
              $inn_fld->setAttribute('model', $model);
            }
          }
        }

        $fld->setAttribute('ttype', $fldDef[$name]['type']);
        if (!$fld->getAttribute('type')) {
          if ($mapped_type = self::$TYPE_MAP[$fldDef[$name]['type']]) {
            $fld->setAttribute('type', $mapped_type);
          }
        }

        switch ($fldDef[$name]['type']) {
          case 'selection':
            break;

          case 'many2one':
            $doc = $fld->ownerDocument;
            $key = $fld->getAttribute('key');

            if($key) {
              $opt_key = $fld->getAttribute('option_key');
              $opt_key = $opt_key ? $opt_key : 'id';

              $q = new \Oerp\Query\Basic($fldDef[$name]['relation'], null,
                array_unique(array($key, $opt_key)));

              //            no-cache query
              $recs = (array)$q->raw(false, true);
              usort($recs, create_function('$a,$b', sprintf('
                return strcmp($a["%s"], $b["%s"]);
              ', $key, $key)));

              foreach ($recs as $rec) {
                $newOpt = $doc->createElement(
                  'option', htmlspecialchars($rec[$key]));

                $newOpt->setAttribute('value', $rec[$opt_key]);
                $newOpt->setAttribute('oerp_rid', $rec['id']);
                $fld->appendChild($newOpt);
              }
            }
            break;
        }
        break;
    }

    $fld->setAttribute('mapped', $fld->getAttribute('mapped') + 1);
  }

  public function getFormEles($node = null, &$form = null)
  {
    $form_send = parent::getFormEles($node, $form);
    $form_send['#attributes'] = array('class' => 'oerp-formview');
    return $form_send;
  }

  protected function getInnerForm($fld, &$form)
  {
    $doc = new \DOMDocument();
    $elForm = $doc->createElement('form');
    $elForm->setAttribute('model', $fld->getAttribute('relation'));
    $elForm->setAttribute('mapped', 2);

    foreach ($fld->childNodes as $chd) {
      $elForm->appendChild($doc->importNode($chd, true));
    }
    $doc->appendChild($elForm);

    $inner_form = new \Oerp\Arch\Form($doc);
    $form[$fld->getAttribute('name')] = $inner_form->getFormEles();
  }

  public function theme(&$form)
  {
    $theme = '';
    if ($title = $this->attr('string')) {
      $theme .= '<div class="caption title">' . $title . '</div>';
    }

    $theme .= parent::theme($form);

    $output = sprintf('
      <div class="oerp-view" oerp_vid="%s" oerp_model="%s">
        <div class="oerp-widget-content ui-widget-content">%s</div>
      </div>
    ', $this->attr('vid'), $this->attr('model'), $theme);

    jquery_ui_add(array('ui.tabs', 'ui.draggable', 'ui.dialog', 'ui.datepicker'));
    \Oerp\Util\Helper::addCss();
    \Oerp\Util\Helper::addJs('jquery.oerpview', 'formview');

    return $output;
  }

  public function send($vals)
  {
    $Rec = new \Oerp\Query\Record($this->dump());
    $resp = $Rec->send($vals);

    if ($resp) {
      drupal_set_message('Order created successfully.');
    }
    else {
      drupal_set_message('Failed to create new order.', 'error');
    }
  }
}