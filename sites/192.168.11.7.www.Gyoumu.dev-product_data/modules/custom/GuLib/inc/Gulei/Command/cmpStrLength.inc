<?php
namespace Gulei\Command;

class cmpStrLength extends \Gulei\Command\WithReference\WithTable
{
  private function cmp($val1, $val2, $op){
    switch($op){
      case 'LT':
        return $val1 < $val2;

      case 'GT':
        return $val1 > $val2;

      case 'LE':
        return $val1 <= $val2;

      case 'GE':
        return $val1 >= $val2;

      case 'EQ':
        return $val1 == $val2;

      case 'NE':
        return $val1 != $val2;
    }
  }

  public function execute($refVar = null)
  {
    $args = $this->getCommandArgs();
    list($rec, $name) = $this->ref->vals('rec', 'name');
    return $this->cmp(mb_strlen($rec[$name]), $args[1], $args[0]);
  }
}