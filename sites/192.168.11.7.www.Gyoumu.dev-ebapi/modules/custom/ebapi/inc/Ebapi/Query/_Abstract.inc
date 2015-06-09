<?php
namespace Ebapi\Query;

abstract class _Abstract
{
  protected $Account;

  public function __construct($Account)
  {
    $this->Account = $Account;
  }
}