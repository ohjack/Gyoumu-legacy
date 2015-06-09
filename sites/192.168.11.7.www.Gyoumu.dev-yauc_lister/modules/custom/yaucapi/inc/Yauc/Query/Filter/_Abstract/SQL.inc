<?php
namespace Yauc\Query\Filter\_Abstract;

abstract class SQL extends \Yauc\Query\_Abstract
{
  abstract public function getSqlWhere();
}