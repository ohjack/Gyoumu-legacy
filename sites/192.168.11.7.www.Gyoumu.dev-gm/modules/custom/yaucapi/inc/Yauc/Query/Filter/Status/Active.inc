<?php
namespace Yauc\Query\Filter\Status;

class Active extends \Yauc\Query\Filter\_Abstract\SQL
{
  public function getSqlWhere()
  {
    $status = array(
      self::AUC_NEW,
      self::AUC_LOADED,
    );

    return sprintf('status IN (%s)', implode(',', $status));
  }
}