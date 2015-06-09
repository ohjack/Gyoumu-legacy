<?php
namespace Gulei\Modifier\Pack;
use Gulei\Modifier as M;

class RowAlt extends _Abstract
{
  protected function init()
  {
    $this->append(
      array(
           new M\Pre('data', 'data'),
           new M\WrapValue('data', 'data', array('out' => array('id'))),
      )
    );
  }
}