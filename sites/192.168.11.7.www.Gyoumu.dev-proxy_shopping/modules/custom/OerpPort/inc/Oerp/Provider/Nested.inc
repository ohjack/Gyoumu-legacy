<?php
namespace Oerp\Provider;

class Nested extends \Gulei\Provider\_Abstract
{
  private $arch;
  private $q;

  public function __construct($arch)
  {
    $this->arch = $arch;
    $this->q = new \Oerp\Query\Nested($this->arch);
  }

  public function getData()
  {
    return (array)$this->q->sum();
  }

  public function getReference()
  {
    return array(
      '_arch' => $this->arch
    );
  }

  public function count()
  {
    return $this->q->execute('search_count');
  }
}