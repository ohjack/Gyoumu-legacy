<?php
namespace Gulei;

class ThemerTable implements \Gulei\Themer\_Interface
{
  private $procHds;
  private $procRecs;

  public function __construct(\Gulei\Processor\Basic $procHds, \Gulei\Processor\Basic $procRecs)
  {
    $this->procHds = $procHds;
    $this->procRecs = $procRecs;
  }

  public function getProcHds()
  {
    return $this->procHds;
  }

  public function getProcRecs()
  {
    return $this->procRecs;
  }

  public function theme()
  {
    $procHds = clone $this->getProcHds();
    $procHds->addModifier(new ModTagize());

    $procRecs = clone $this->getProcRecs();
    $procRecs->addModifier(new ModTagize());

    return \theme('table', $procHds->getOutput(), $procRecs->getOutput());
  }
}

