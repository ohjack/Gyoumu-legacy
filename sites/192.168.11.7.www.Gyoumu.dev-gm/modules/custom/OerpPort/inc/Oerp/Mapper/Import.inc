<?php
namespace Oerp\Mapper;

class Import extends \Gulei\Mapper\Basic
{
  protected function appendValue(&$recs, $ind, $name, $val, $field)
  {
    $ref = array(
      'ind' => $ind,
      'name' => $name,
      'val' => $val,
      'field' => $field,
      'rec' => $this->recs[$ind],
    );

    $refVar = new \Gulei\Manipulator\RefVariable();
    $refVar->addRefVar(array('recs' => &$recs));

    $Apps = array(
      new \Oerp\Mapper\Appender\Many2one($ref),
      new \Oerp\Mapper\Appender\One2many($ref),
      new \Oerp\Mapper\Appender\Plain($ref),
    );

    foreach($Apps as $App){
      if($App->isApplicable()){
        $App->execute($refVar);
        break;
      }
    }
  }
}