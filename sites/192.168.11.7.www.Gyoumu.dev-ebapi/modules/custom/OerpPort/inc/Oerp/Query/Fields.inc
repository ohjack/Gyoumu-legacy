<?php
namespace Oerp\Query;

class Fields extends \Oerp\Query\Basic
{
  private $vals;

  function __construct($mod)
  {
    parent::__construct(
      'ir.model.fields', array(array('model', '=', $mod))
    );
  }

  public function getVals()
  {
    return $this->vals;
  }

  public function setVal($key, $val)
  {
    $fld = $this->getDef($key);

    switch ($fld['ttype']) {
      case 'boolean':
        $this->vals[$key] = (bool)$val;
        break;
      case 'many2one':
      case 'one2many':
      case 'many2many':
      default:
        $this->vals[$key] = $val;
        break;
    }
  }

  public function mergeVal($key, $val)
  {
    $fld = $this->getDef($key);

    switch ($fld['ttype']) {
      case 'one2many':
      case 'many2many':
        $this->vals[$key] = array_merge(
          $this->vals[$key], (array)$val);
        break;
    }
  }

  public function getModel()
  {
    $cri = $this->getCriteria();
    return $cri[0][2];
  }

  public function getFields()
  {
    return $this->raw()->bunchKey('name');
  }

  public function defaults()
  {
    $Qry = new \Oerp\Query\Basic($this->getModel(), null, $this->getFields());
    return $Qry->defaults();
  }

  static public function getDef($mod, $cri = null)
  {
    $q = new \Oerp\Query\Fields($mod);
    $defs = $q->exec(array('fields_get'));

    if (is_string($cri)) {
      if ($defs[$cri]) {
        return new \Oerp\Util\_Array($defs[$cri]);
      } else {
        return null;
      }
    }
    if (is_array($cri)) {
      $keys = array_fill_keys($cri, null);

      return new \Oerp\Util\_Array(
        array_intersect_key($defs->getArrayCopy(), $keys)
      );
    }

    foreach ($defs as &$def) {
      if ($def['type'] == 'selection') {
        $opts = $def['selection'];
        $def['selection'] = array();

        foreach ($opts as $opt) {
          $def['selection'][$opt[0]] = $opt[1];
        }
      }
    }

    return $defs;
  }

  public function isRelType($name)
  {
    $def = $this->getDef($name);
    if (in_array($def['ttype'],
                 array('one2many', 'many2one', 'many2many'))
    ) {

      return true;
    } else {
      return false;
    }
  }

  public function getRels($mod)
  {
    $rels = array();
    $flds = (array)$this->getDef($this->getModel())->filter(
      'return $item["relation"] == "' . $mod . '";');

    foreach ($flds as $fld) {
      $rels[] = array(
        'mod' => $fld['relation'],
        'ttype' => $fld['ttype'],
        'key' => $fld['name'],
      );
    }

    return $rels;
  }

  public function getFormEles()
  {
    $flds = $this->getDef($this->getModel());
    $eles = array();

    foreach ($flds as $fld) {
      $eles[$fld['name']]['#title'] = $fld['field_description'];

      switch ($fld['ttype']) {
        case 'text':
          $eles[$fld['name']]['#type'] = 'textarea';
          break;
        case 'char':
          $eles[$fld['name']]['#type'] = 'textfield';
          break;
        case 'float':
          $eles[$fld['name']]['#type'] = 'textfield';
          break;
        case 'integer':
          $eles[$fld['name']]['#type'] = 'textfield';
          break;
        case 'boolean':
          $eles[$fld['name']]['#type'] = 'checkboxes';
          $eles[$fld['name']]['#options'] = array('1' => '');
          break;
        case 'many2one':
          $name_fld = (strpos($fld['relation'], 'x_') === 0) ? 'x_name' : 'name';
          $Qry = new \Oerp\Query\Basic($fld['relation'], null, array($name_fld));
          $opts = (array)$Qry->raw()->promote('id', true);
          $opts[0] = '-None-';
          ksort($opts);

          $eles[$fld['name']]['#type'] = 'select';
          $eles[$fld['name']]['#options'] = $opts;

          break;
        case 'many2many':
        case 'one2many':
          //        need reviews
          $name_fld = (strpos($fld['relation'], 'x_') === 0) ? 'x_name' : 'name';
          $Qry = new \Oerp\Query\Basic($fld['relation'], null, array($name_fld));
          $opts = $Qry->raw()->promote('id', true);

          $eles[$fld['name']]['#type'] = 'checkboxes';
          $eles[$fld['name']]['#options'] = $opts;
          break;
        default:
          continue;
      }

      if ($fld['required']) {
        $eles[$fld['name']]['#required'] = true;
      }
    }
    return $eles;
  }

  static public function getCaption($fld, $arch)
  {
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

  static public function getEle($args)
  {
    $Ele = \Oerp\Form\Element\Factory::createElement($args);
    return $Ele->getEntry();
  }

  static public function getSearchEle($fld, $arch, $default = null)
  {
    $args = array(
      'field' => $fld,
      'arch' => $arch,
    );

    switch ($fld['type']) {
      case 'char':
        $ele = self::getEle($args);
        $ele['#attributes']['op'] = 'ilike';

        break;

      case 'int':
        $ele = self::getEle($args);
        $ele['#attributes']['op'] = '=';
        break;

      case 'many2one':
        $ele = self::getEle($args);
        $ele['#type'] = 'textfield';
        $ele['#attributes']['op'] = 'ilike';
        unset($ele['#value']);
        break;

      case 'one2many':
        $ele['#type'] = 'textfield';
        $ele['#title'] = self::getCaption($fld, $arch);
        $ele['#description'] = $fld['help'];
        $ele['#attributes']['op'] = 'ilike';
        break;

      case 'float':
        $eleTpl = self::getEle($args);
        $eleTpl['#default_value'] = '0.0';
        unset($eleTpl['#required']);

        $ele['GE'] = $eleTpl;

        $ele['LE'] = $eleTpl;
        $ele['LE']['#title'] = '<sup> </sup>~';
        unset($ele['LE']['#description']);
        break;

      case 'date':
      case 'datetime':
        $eleTpl = self::getEle($args);
        $eleTpl['#default_value'] = '';
        $eleTpl['#attributes']['widget'] = 'datepicker';

        unset(
          $eleTpl['#required'],
          $eleTpl['#attributes']['readonly']
        );

        $ele['GE'] = $eleTpl;

        $ele['LE'] = $eleTpl;
        $ele['LE']['#title'] = '<sup> </sup>~';
        unset($ele['LE']['#description']);
        break;

      case 'boolean':
        $ele = array(
          '#type' => 'select',
          '#title' => self::getCaption($fld, $arch),
          '#description' => $fld['help'],
          '#attributes' => array('op' => '='),
          '#options' => array(
            'null' => '',
            'yes' => 'Yes',
            'no' => 'No'
          ),
          '#default_value' => 'null',
        );
        break;

      case 'selection':
        $ele = self::getEle($args);

        $ele['#options'] = array_merge(
          array('_null_' => '<None>'),
          $ele['#options']
        );

        $ele['#default_value'] = '_null_';
        $ele['#attributes']['op'] = '=';
        break;

      default:
        $ele = self::getEle($args);
    }

    unset(
    $ele['#required'],
    $ele['#attributes']['readonly'],
    $ele['#attributes']['disabled']
    );

    if (!is_null($default)) {
      switch ($fld['type']) {
        case 'char':
        case 'many2one':
        case 'one2many':
        $ele['#default_value'] = $default['ilike'];
          break;

        case 'int':
          $ele['#default_value'] = $default['='];
          break;

        case 'boolean':
          $ele['#default_value'] = $default['='] ? 'yes' : 'no';
          break;

        case 'float':
          if (isset($default['='])) {
            $default['>='] = $default['='];
            $default['<='] = $default['='];
          }
          $ele['GE']['#default_value'] = sprintf('%.1f', (float)$default['>=']);
          $ele['LE']['#default_value'] = sprintf('%.1f', (float)$default['<=']);
          break;

        case 'datetime':
          $ele['GE']['#default_value'] = $default['>='];
          $ele['LE']['#default_value'] = $default['<='];
          break;

        case 'selection':
          $ele['#default_value'] = $default['='];
          break;
      }
    }
    return $ele;
  }
}