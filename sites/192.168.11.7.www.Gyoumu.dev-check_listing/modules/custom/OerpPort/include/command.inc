<?php
namespace Oerp\Command;

class existValue extends \Gulei\Command\AbstractCommandWithReference
{
  public function execute($refVar = null)
  {
    list($rec, $name) = array(
      $this->ref->val('rec'),
      $this->ref->val('name'),
    );

    $args = $this->getCommandArgs();
    $cri = ($cri_src = json_decode($args[1])) ? $cri_src : array();

    $cri[] = array(
      $args[2], '=', $rec[$name]
    );

    $q = new \OerpQuery($args[0], $cri, array('id'));
    $resp = (array)$q->raw();

    return !empty($resp);
  }
}
