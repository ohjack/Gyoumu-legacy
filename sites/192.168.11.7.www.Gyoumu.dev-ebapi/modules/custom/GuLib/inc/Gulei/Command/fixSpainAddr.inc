<?php
namespace Gulei\Command;

class fixSpainAddr extends \Gulei\Command\WithReference\_Abstract
{
  public function execute($refVar = null)
  {
    list($rec, $name) = $this->ref->vals('rec', 'name');

    $rs = preg_replace(
      array(
        '/nº/',
        '/(\d*1)(º|ª)/',
        '/(\d*2)(º|ª)/',
        '/(\d*3)(º|ª)/',
        '/([4-9]|\d{2,})(º|ª)/',
      ),
      array(
        'No.',
        '\1st',
        '\1nd',
        '\1rd',
        '\1th',
      ),
      $rec[$name]
    );
    return $rs;
  }
}