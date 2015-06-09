<?php
namespace Yauc\Query\Filter;

class Seller extends \Yauc\Query\Filter\_Abstract\SQL
{
  private $seller;

  public function __construct($seller)
  {
    $this->seller = $seller;
  }

  public function getSqlWhere()
  {
    return sprintf('seller_id = "%s"', $this->seller);
  }
}