<?php
namespace Oerp\Query\Operation;

class Factory
{
  public function getOperations($arch, $recs)
  {
    $Arch = new \Oerp\Arch\Element($arch);
    $Proc = new \Gulei\Processor\Basic();

    $Proc
        ->applyArchReference($Arch->dump())
        ->setDataProvider(new \Gulei\Provider\Lambda($recs));

    $Proc->addModifiers(array(
      new \Oerp\Modifier\PrepareSend()
    ));

    return $Proc->getOutput();
  }
}