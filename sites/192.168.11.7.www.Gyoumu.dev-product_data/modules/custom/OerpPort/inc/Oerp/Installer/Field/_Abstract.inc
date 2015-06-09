<?php
namespace Oerp\Installer\Field;

abstract class _Abstract extends \Oerp\Installer\_Abstract
{
  protected function getTablename()
  {
    return 'ir.model.fields';
  }

  protected function getParamMapping()
  {
    return array('name', 'field_description', 'model_id', 'ttype');
  }

  protected function getDefaultParam()
  {
    return array('state' => 'manual');
  }

  protected function getDefCri()
  {
    return array(
      array('name', '=', $this->param['name']),
      array('model_id', '=', $this->param['model_id']),
    );
  }

  protected function init()
  {
    if(is_numeric($this->param['model_id'])){
      return null;
    }

    $q = new \Oerp\Query\Basic(
      'ir.model',
      array(array('model', '=', $this->param['model_id'])),
      array('id')
    );

    $resp = $q->raw();
    $this->param['model_id'] = $resp[0]['id'];
  }

  protected function update($def)
  {
    $q = $this->createQuery(null, null, $def['id']);
    $q->write(array('field_description' => $this->param['field_description']));
  }
}