<?php
namespace Oerp\Product\Provider;

class PackagingEx implements \Gulei\Provider\_Interface
{
  private $prov;

  public function __construct()
  {
    $this->prov = $this->getProvider();
  }

  private function getSrcArch()
  {
    $arch = \Gulei\File\Helper::getPathContent('module', 'oerp_product', '/xml/packaging.xml');
    return $arch;
  }

  public function getArch()
  {
    $arch = \Gulei\File\Helper::getPathContent('module', 'oerp_product', '/xml/PackagingEx.xml');
    return $arch;
  }

  private function getProvider()
  {
    $prov = new \Oerp\Provider\Nested($this->getSrcArch());
    return $prov;
  }


  public function getData()
  {
    $proc = new \Gulei\Processor\Basic();
    $proc->setDataProvider($this->prov);

    $proc->addModifiers(
      array(
        new \Oerp\Product\Modifier\FlatPackaging(),
      )
    );

    return \Gulei\_Array::lambda($proc->getOutput(), "Oerp\\Product\\_Array\\Command\\sortSKU");
  }

  public function getReference()
  {
    return $this->prov->getReference();
  }

  public function count()
  {
    return $this->prov->count();
  }
}