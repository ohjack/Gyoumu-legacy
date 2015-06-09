<?php
namespace Oerp\Installer\View;

abstract class _Abstract extends \Oerp\Installer\_Abstract
{
  abstract protected function getFilepath();
  abstract protected function getName();
  abstract protected function getModel();

  public function __construct()
  {
    return parent::__construct($this->getName(), $this->getModel());
  }

  protected function getTablename()
  {
    return 'ir.ui.view';
  }

  protected function init()
  {
    $this->param['arch'] = $this->getArch();
  }

  protected function getParamMapping()
  {
    return array('name', 'model', 'type');
  }

  protected function getDefaultParam()
  {
    return array(
      'priority' => 6,
    );
  }

  protected function getDefCri()
  {
    return array(
      array('name', '=', $this->param['name']),
    );
  }

  protected function getArch()
  {
    $fp = str_replace('.inc', '.xml', $this->getFilepath());
    return file_get_contents($fp);
  }

  protected function update($def)
  {
    $q = $this->createQuery(null, null, $def['id']);
    $q->write(array('arch' => $this->getArch()));
  }
}