<?php
namespace Yauc\Lister\Command;

class mapStatus extends \Gulei\Command\WithReference\WithTable
{
  public function execute($refVar = null)
  {
    list($rec, $name) = array(
      $this->ref->val('rec'), $this->ref->val('name')
    );

    switch($rec[$name])
    {
      case \Yauc\Lister\Provider\_Abstract::Q_NEW:
        return 'New';

      case \Yauc\Lister\Provider\_Abstract::Q_FAILED:
        return 'Error';

      case \Yauc\Lister\Provider\_Abstract::Q_DONE:
        return 'Done';

      case \Yauc\Lister\Provider\_Abstract::Q_CANCEL:
        return 'Canceled';
    }
  }
}