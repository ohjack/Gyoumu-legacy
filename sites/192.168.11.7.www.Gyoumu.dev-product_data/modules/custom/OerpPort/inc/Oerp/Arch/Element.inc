<?php
namespace Oerp\Arch;

class Element extends \Gulei\Arch\Param\Helper
{
  private $fields;

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

  function __construct($arch, $override = array())
  {
    parent::__construct($arch, $override);

    $params = $this->getParam();
    $params['oerp:model'] = $this->getInitModel($params, $override);
    $params['oerp:criteria'] = $this->getInitCriteria($params, $override);
    $params['oerp:fields'] = $this->getInitFields($params, $override);
    $params['oerp:ids'] = $this->getInitIds($params, $override);

    $this->getArch()->setAttribute('model', $params['oerp:model']);

    $this->param = $params;
    $this->mapEles();
  }

  protected function getInitModel($params, $override)
  {
    $model = $this->attr('model');
    $model = ($over = $this->attr('relation')) ? $over : $model;
    $model = ($over = $params['model']) ? $over : $model;
    $model = ($over = $params['oerp:model']) ? $over : $model;
    $model = ($over = $override['model']) ? $over : $model;
    $model = ($over = $override['oerp:model']) ? $over : $model;
    return $model;
  }

  protected function getInitCriteria($params, $override)
  {
    $cri = \Oerp\Form\Element\SearchPanel::decodeCri($params['oerp:criteria']);
    $cri = ($over = $this->getArch()->getAttribute('criteria'))
        ? $over : $cri;

    $cri = ($over = $override['criteria']) ? $over : $cri;
    $cri = ($over = $override['oerp:criteria']) ? $over : $cri;
    $cri = \Oerp\Form\Element\SearchPanel::decodeCri($cri);

    $cri_ex = $override['oerp:cri_ex'];

    if(is_string($cri_ex)){
      $cri_ex = \Oerp\Form\Element\SearchPanel::decodeCri($override['oerp:cri_ex']);
    }

    if($cri_ex){
      foreach ($cri_ex as $each) {
        $cri[] = $each;
      }
    }
    return $cri;
  }

  protected function getInitFields($params, $override)
  {
    $flds = $this->getFields($this->getArch());
    $flds = ($over = $params['oerp:fields']) ? $over : $flds;

    $over_flds = $override['oerp:fields'];
    $over_flds = is_string($override) ? json_decode($over_flds) : $over_flds;
    $flds = $over_flds ? $over_flds : $flds;
    return $flds;
  }

  protected function getInitIds($params, $override)
  {
    $ids = $params['oerp:ids'];
    $ids = is_string($ids) ? json_decode($ids) : $ids;
    $ids = ($over = $this->getArch()->getAttribute('ids'))
        ? $over : $ids;

    $over = is_string($override['oerp:ids'])
        ? json_decode($override['oerp:ids'])
        : $override['oerp:ids'];

    $ids = $over ? $over : $ids;
    return $ids;
  }

  protected function getFieldEleList()
  {
    $xp = <<<XPATH
/form//field[not(self::*/ancestor::field[1])][not(self::*/ancestor::tree[1])]
| /form//tree
| /field/field
| /tree/field
XPATH;

    return $this->xp($xp, $this->getArch());
  }

  public function getFields()
  {
    $flds = array();

    foreach($this->getFieldEleList() as $eleFld){
      $flds[] = $eleFld->getAttribute('name');
    }

    return $flds;
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

  protected function mapEle($fld)
  {
    if ($fld->getAttribute('OerpArchElement_mapped')) {
      return;
    }

    $name = $fld->getAttribute('name');
    $model = $fld->getAttribute('model');
    $fldDef = $this->getFieldDef($model);

    switch ($fld->nodeName) {
      case 'form':
        foreach ($this->xp('.//field | ./tree', $fld) as $inn_fld) {
          if ($fld->hasAttribute('relation')) {
            $inn_fld->setAttribute('model', $fld->getAttribute('relation'));
          }
          else {
            $inn_fld->setAttribute('model', $model);
          }
        }
        break;

      case 'tree':
        if(!$fldDef[$name]['relation']){
          $fld->setAttribute('relation', $fld->getAttribute('model'));
        }
        else{
          $fld->setAttribute('relation', $fldDef[$name]['relation']);
        }

        $relation = $fld->getAttribute('relation');

        if($fldDef[$name]['type']){
          $fld->setAttribute('ttype', $fldDef[$name]['type']);
        }

        $xp = './/field[not(self::*/ancestor::field[1])]';

        foreach ($this->xp($xp, $fld) as $inn_fld) {
          $inn_fld->setAttribute('model', $relation);
        }
        break;

      case 'field':
        if (!$name)
          return;

        $fld->setAttribute('ttype', $fldDef[$name]['type']);

        if ($fldDef[$name]['relation'] && $fldDef[$name]['relation'] != 'NULL') {
          $fld->setAttribute('relation', $fldDef[$name]['relation']);
        }

        if (!$fld->getAttribute('string')) {
          $fld->setAttribute('string',
                             ($fldDef[$name]) ? $fldDef[$name]['field_description'] : $name);
        }

        if ($fld->hasChildNodes()) {
          foreach ($this->xp('.//field', $fld) as $inn_fld) {
            if ($fld->hasAttribute('relation')) {
              $inn_fld->setAttribute('model', $fld->getAttribute('relation'));
            }
            else {
              $inn_fld->setAttribute('model', $model);
            }

            if ($anchor = $inn_fld->getAttribute('anchor')) {
              $inn_fld->setAttribute('anchor', $anchor . '.' . $name);
            }
            else {
              $inn_fld->setAttribute('anchor', $name);
            }
          }
        }
        break;
    }

    $fld->setAttribute('OerpArchElement_mapped', '1');
  }

  protected function mapEles()
  {
    $this->walk(array($this, 'mapEle'));
  }

  protected function isWrapperField($eleFld)
  {
    return !!$this->xp('.//field[1]', $eleFld)->length;
  }
}
