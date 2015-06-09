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

  protected function createInstField()
  {
    $args = func_get_args();
    $type = array_splice($args, 2, 1);
    $type = $type[0];
    $args[] = $this->getModel();

    $classname = "Oerp\\Installer\\Field\\" . ucfirst($type);
    $reflect = new \ReflectionClass($classname);
    return $reflect->newInstanceArgs($args);
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