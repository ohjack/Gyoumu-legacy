<?php
namespace Oerp\Product\Page;

class Meta extends \Gulei\Page\Abstract_Content
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function content()
  {
    $param = array(
      'mark_translated' => false,
      'nl2br' => false,
    );

    $prov = new \Oerp\Product\Provider\Meta();
    $t = new \Gulei\Table\Arch($prov->getArch(), $prov, $param);
    return $t->theme();
  }
}