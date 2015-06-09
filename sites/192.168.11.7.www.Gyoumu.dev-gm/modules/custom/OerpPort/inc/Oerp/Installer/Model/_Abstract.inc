<?php
namespace Oerp\Installer\Model;

abstract class _Abstract extends \Oerp\Installer\_Abstract
{
  abstract protected function getModel();
  abstract protected function getInstFields();

  public function __construct()
  {
    parent::__construct($this->getModel(), $this->getModel());
  }

  protected function getTablename()
  {
    return 'ir.model';
  }

  protected function getParamMapping()
  {
    return array('model', 'name');
  }

  protected function getDefaultParam()
  {
    return array();
  }

  protected function getDefCri()
  {
    return array(
      array('model', '=', $this->param['model']),
    );
  }

  protected function createInstField($name, $label, $type)
  {
    $classname = "Oerp\\Installer\\Field\\" . ucfirst($type);
    return new $classname($name, $label, $this->getModel());
  }

  public function exe()
  {
    parent::exe();

    foreach($this->getInstFields() as $inst){
      $inst->exe();
    }
  }

  protected function update($def)
  {
    $q = $this->createQuery(null, null, $def['id']);
    $q->write(array('name' => $this->param['name']));
  }
}