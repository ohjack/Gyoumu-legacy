<?php
namespace Oerp\Product\Provider;

abstract class Abstract_Meta extends \Gulei\Provider\_Abstract
{
  protected $prov;

  abstract public function getArch();

  public function __construct()
  {
    $this->prov = $this->getProvider();
  }

  public function getParam() {
    return array(
      'arch' => $this->getArch(),
      'oerp:criteria' => array(array('state', '<>', 'obsolete')),
      'no-blank-tail' => true,
      'flag' => array(
        'no-cache' => TRUE,
      )
    );
  }

  protected function getProvider()
  {
    $prov = new \Oerp\Provider\TreeRecords($this->getParam());
    return $prov;
  }


  public function getData() {
    $proc = new \Gulei\Processor\Basic();
    $proc->setDataProvider($this->prov);

    $bool_flds = array(
      'product_tmpl_id.purchase_ok',
      'product_tmpl_id.sale_ok',
    );

    $proc->addModifiers(
      array(
           new \Gulei\Modifier\VisualizeBoolean(null, null, $bool_flds)
      )
    );

    $recs = $proc->getOutput();
    \Oerp\Product\_Array\Command\sortSKU::sort($recs);
    return $recs;
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