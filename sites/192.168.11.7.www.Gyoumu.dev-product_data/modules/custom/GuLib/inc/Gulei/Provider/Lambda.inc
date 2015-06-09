<?php
namespace Gulei\Provider;

class Lambda extends \Gulei\Provider\_Abstract
{
  private $data;

  function __construct($data)
  {
    $this->data = $data;
  }

  public function getData()
  {
    return $this->data;
  }

  public function getReference()
  {
    return array('_data' => $this->data);
  }

  public function count()
  {
    return count($this->data);
  }
}